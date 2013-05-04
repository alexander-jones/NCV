#include "ncvcanvas.h"


NCVCanvas::NCVCanvas(  const QGLFormat& format, QWidget* parent )
    : QGLXCanvas( format,parent )
{
    m_versionCapable = QGLFormat::openGLVersionFlags().testFlag(QGLFormat::OpenGL_Version_3_3);

    this->installEventFilter(this);
    if (!m_versionCapable)
    {
        qDebug() << "Version Incapable";
        return;
    }

    this->setAutoBufferSwap(false);
    this->setAutoFillBackground(false);
    m_leftMouseDown = false;
    m_rightMouseDown = false;
    m_legendShowTime = 1300.0;
    m_moveSpeed = 2000;
    m_turnSpeed = 0.005f;
    m_worldSize = QVector3D(5000,5000,5000);
    m_worldCenter  =QVector3D(0,0,0);
    m_camera.setView(QVector3D(-m_worldSize.x(),m_worldSize.y(),-m_worldSize.z()), QVector3D(0,0,1),QVector3D(0,1,0));
    m_camera.setProjection(35.0, 4.0/3.0, 0.1f, m_worldSize.length()*2.0f );

    m_renderNeurons= true;
    m_renderConnections= true;
    m_initialized = false;
    this->setMouseTracking(true);//Qt::WA_Hover
    srand(time(NULL));
    m_idleTimer.start();
	m_renderDirty= true;
	m_selectionFlags = RenderDeselected;
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_connections = NULL;
    m_neurons = NULL;

}

void NCVCanvas::m_drawLegend()
{

    GLuint objectID= 0;
    m_frameBufferObject.getTextureData(m_maps["id"],QRect(m_mousePosition.x(),m_mousePosition.y(),1,1),&objectID);
    glFinish();
    if (objectID == 0)
        return;

    const int Padding = 6;

    QTextDocument textDocument;
    textDocument.setDefaultStyleSheet("* { color: #FFEFEF }");
    QString html = "";
    int offset;
    QMap<QString, NCVAttribute *> attributes;

    if (objectID  < (GLuint)m_neurons->count())
    {
        html += "<h4 align=\"center\">Neuron (" + QString::number(objectID) + ") </h4>";
        offset = objectID - 1;
        attributes = m_neurons->attributes();

    }
    else
    {
        html += "<h4 align=\"center\">Connection (" + QString::number(objectID) + ")</h4>";
        offset = objectID - m_neurons->count() ;
        attributes = m_connections->attributes();
    }

    for (QMap<QString, NCVAttribute *>::iterator it = attributes.begin(); it !=attributes.end(); it++)
    {
        NCVAttribute * attribute = it.value();
        if ( attribute->type() == Continuous)
        {
            NCVContinuousAttribute * attrib = dynamic_cast<NCVContinuousAttribute *>(attribute);
            QVector<GLfloat> data = attrib->data();
            html += "\n<h4 align=\"center\">" + it.key()+ ": " +QString::number(data[offset]) + "</h4>";

        }
        else if ( attribute->type() == Discrete)
        {

            NCVDiscreteAttribute * attrib = dynamic_cast<NCVDiscreteAttribute *>(attribute);
            QVector<QString> data = attrib->stateValues(offset,1);

            html += "\n<h4 align=\"center\">" + it.key()+ ": " +data[0]+"  </h4>";

        }
    }

    textDocument.setHtml(html);
    textDocument.setTextWidth(textDocument.size().width());

    QRect rect(QPoint(0, 0), textDocument.size().toSize()
                             + QSize(2 * Padding, 2 * Padding));
    QVector2D topLeft = m_mousePosition;
    if (m_mousePosition.x() + rect.width() > m_width)
        topLeft.setX(m_mousePosition.x() - rect.width());
    if (m_mousePosition.y() + rect.height() > m_height)
        topLeft.setY(m_mousePosition.y() - rect.height());

    //m_painter.translate(m_width - rect.width() - Margin, m_height - rect.height() - Margin);
    m_painter.translate(topLeft.x(),topLeft.y());
    m_painter.setPen(QColor(255, 239, 239));
    m_painter.setBrush(QColor(255, 0, 0, 125));
    m_painter.drawRect(rect);

    m_painter.translate(Padding, Padding);
    textDocument.drawContents(&m_painter);
}
NCVCanvas::~NCVCanvas()
{
}


void NCVCanvas::initializeGL()
{

    if (!isValid())
    {
        invalidGraphicsConfigurationDetected();
        return;
    }
    glewInit();

    QGLFormat glFormat = this->format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DITHER);
    glDisable(GL_MULTISAMPLE);
    glLineWidth(2);
    glDepthFunc(GL_LEQUAL);


    // initialize target bufferm_neuronsToCreate
    m_frameBufferObject.create();

    QString names[3] = {"diffuse","id","depth"};
    GLenum formats[3] = {GL_RGB8,GL_R32UI,GL_DEPTH_COMPONENT32};
    GLuint samples[3] = {1,1,1};
    GLint maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

    for(int i = 0 ; i < 3; i ++)
    {
        m_maps.insert(names[i],QGLXTexture2D());
        m_maps[names[i]].create();
        m_maps[names[i]].allocate(1000,1000,formats[i],samples[i]);
        m_maps[names[i]].bind();
        m_maps[names[i]].setMinFilter(QGLXTexture2D::Nearest);
        m_maps[names[i]].setMagFilter(QGLXTexture2D::Nearest);
        m_maps[names[i]].setWrapFunction(QGLXTexture2D::Clamp,QGLXTexture2D::Clamp);
        m_maps[names[i]].release();

    }

    QGLFormat f = format();
    QVector3D screenVerts[4] = {QVector3D(-1,-1,0.5),QVector3D(1,-1,0.5),QVector3D(1,1,0.5),QVector3D(-1,1,0.5)};
    QVector2D screenCoords[4] = {QVector2D(0,0),QVector2D(1,0),QVector2D(1,1),QVector2D(0,1)};
    m_screenVertices.create();
    m_screenVertices.bind(QGLXBuffer::ArrayBuffer);

    m_screenVertices.allocate(&screenVerts[0],4 * sizeof(QVector3D));
    m_screenVertices.release();


    m_screenCoords.create();
    m_screenCoords.bind(QGLXBuffer::ArrayBuffer);
    m_screenCoords.allocate(&screenCoords[0],4 * sizeof(QVector2D));
    m_screenCoords.release();

    m_skySphere.create(m_camera.farPlane() * 10000 ,QImage(":/assets/gray.jpg"),QVector2D(10,10));

    this->startTimer(1);
    m_initialized = true;
    initialized();
}

void NCVCanvas::timerEvent(QTimerEvent * e)
{

    if (!isValid())
        return;
    e->accept();
    // perform refresh
    update();
    GLuint err = glGetError();
    if (err != 0)
        qDebug() <<QGLXCore::getErrorString(err);

}


void NCVCanvas::setBackgroundImage(QImage image)
{
    //m_skySphere.setImage(image);
}

void NCVCanvas::resizeGL( int w, int h )
{
    if (!isValid())
        return;

	m_renderDirty = true;
    // Set the viewport to window dimensions
    m_width = w;
    m_height = h;
    glViewport( 0, 0, w, qMax( h, 1 ) );
    int targetWidth = m_width;
    int targetHeight = m_height;

    m_camera.setAspectRatio((float)m_width/m_height);

    for (QMap<QString,QGLXTexture2D>::iterator it = m_maps.begin();it != m_maps.end(); it++)
        it.value().setSize(targetWidth,targetHeight);
}

bool NCVCanvas::isValid()
{
    return m_versionCapable && QGLWidget::isValid();
}

bool NCVCanvas::isInitialized()
{
    return isValid() && m_initialized;
}


void NCVCanvas::m_performRegularRender()
{

    // bind fbo and related targets
    m_frameBufferObject.bind();
    m_frameBufferObject.enableColorAttachments(1);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["depth"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Depth);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_skySphere.draw(m_camera);

    m_frameBufferObject.enableColorAttachments(1);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    glClear( GL_COLOR_BUFFER_BIT);

    m_frameBufferObject.enableColorAttachments(2);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);


    // clear targets
    if (m_renderNeurons && m_neurons != NULL)
    {
        m_neurons->bind(m_camera);
        m_neurons->draw();
        m_neurons->release();
    }
    if (m_renderConnections && m_connections != NULL)
    {
        m_connections->bind(m_camera);
        m_connections->draw();
        m_connections->release();
    }

    m_maps["id"].release();
    m_frameBufferObject.enableColorAttachments(1);
    m_maps["depth"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Depth);


    if (m_renderNeurons && m_neurons != NULL)
    {
        m_neurons->bindSilhouettes(m_camera);
        m_neurons->draw();
        m_neurons->releaseSilhouettes();
    }

    if (m_renderConnections && m_connections != NULL)
    {
        m_connections->bindSilhouettes(m_camera);
        m_connections->draw();
        m_connections->releaseSilhouettes();
    }


    // release frame buffer and related targets
    m_maps["diffuse"].release();
    m_maps["depth"].release();
    m_frameBufferObject.enableColorAttachments(0);
    m_frameBufferObject.release();


}


void NCVCanvas::m_performSelectionRender()
{

    // bind frame buffer and all first pass targets
    m_frameBufferObject.bind();
    m_frameBufferObject.enableColorAttachments(1);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["depth"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Depth);


    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_skySphere.draw(m_camera);

    m_frameBufferObject.enableColorAttachments(1);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    glClear( GL_COLOR_BUFFER_BIT);

    m_frameBufferObject.enableColorAttachments(2);
    m_maps["diffuse"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color0);
    m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);

    if (m_renderNeurons && m_neurons != NULL)
    {
        m_neurons->bind(m_camera);
        for (QVector<Range>::iterator it = m_ranges.begin(); it != m_ranges.end() ; it++)
        {
            Range range = *it;
            if (range.start > m_neurons->count())
                break;
            else if (range.end > m_neurons->count())
            {
                m_neurons->drawSubset(range.start-1,m_neurons->count()-range.start);
                break;
            }
            else
                m_neurons->drawSubset(range.start-1,range.end-range.start);
        }

        m_neurons->release();
    }
    if (m_renderConnections && m_connections != NULL)
    {

        m_connections->bind(m_camera);
        for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
        {
            Range range = *it;

            if (range.end <= m_neurons->count())
                continue;
            else if (range.start <= m_neurons->count())
                m_connections->drawSubset(0,range.end -(m_neurons->count() +1));
            else
                m_connections->drawSubset(range.start-1 - m_neurons->count(),range.end-range.start);

        }

        m_connections->release();
    }
	 
    m_frameBufferObject.enableColorAttachments(1);

    if (m_renderNeurons && m_neurons != NULL)
    {
        m_neurons->bindSilhouettes(m_camera);
        for (QVector<Range>::iterator it = m_ranges.begin(); it != m_ranges.end() ; it++)
        {
            Range range = *it;
            if (range.start > m_neurons->count())
                break;
            else if (range.end > m_neurons->count())
            {
                m_neurons->drawSubset(range.start-1,m_neurons->count()-range.start);
                break;
            }
            else
                m_neurons->drawSubset(range.start-1,range.end-range.start);
        }

        m_neurons->releaseSilhouettes();
    }
    if (m_renderConnections && m_connections != NULL)
    {

        m_connections->bindSilhouettes(m_camera);
        for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
        {
            Range range = *it;

            if (range.end <= m_neurons->count())
                continue;
            else if (range.start <= m_neurons->count())
                m_connections->drawSubset(0,range.end -(m_neurons->count() +1));
            else
                m_connections->drawSubset(range.start-1 - m_neurons->count(),range.end-range.start);

        }
        m_connections->releaseSilhouettes();

    }

    if ((int)m_selectionFlags & (int)RenderDeselected)
    {
		
		m_frameBufferObject.enableColorAttachments(2);
		m_maps["id"].bind(QGLXTexture2D::Draw,QGLXTexture2D::Color1);

        if (m_renderNeurons && m_neurons != NULL)
        {
            m_neurons->bind(m_camera,true);
            int previousEnd = 1;
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.start > m_neurons->count())
                    break;
                else if (range.start > previousEnd)
                    m_neurons->drawSubset(previousEnd-1,range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < m_neurons->count())
                m_neurons->drawSubset(previousEnd-1,m_neurons->count()-previousEnd);

            m_neurons->release();
        }
        if (m_renderConnections  && m_connections != NULL)
        {
            m_connections->bind(m_camera,true);
            int previousEnd = m_neurons->count() +1;
            int endOfItems = m_neurons->count() + m_connections->count();
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.end <= m_neurons->count())
                    continue;
                else if (range.start > previousEnd)
                    m_connections->drawSubset(previousEnd-1- m_neurons->count(),range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < endOfItems)
                m_connections->drawSubset(previousEnd-1- m_neurons->count(),endOfItems-previousEnd);

            m_connections->release();


        }
		
    m_frameBufferObject.enableColorAttachments(1);
        if (m_renderNeurons &&  m_neurons != NULL )
		{
			m_neurons->bindSilhouettes(m_camera);
            int previousEnd = 1;
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.start > m_neurons->count())
                    break;
                else if (range.start > previousEnd)
                    m_neurons->drawSubset(previousEnd-1,range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < m_neurons->count())
                m_neurons->drawSubset(previousEnd-1,m_neurons->count()-previousEnd);


			m_neurons->releaseSilhouettes();
		}
        if (m_renderConnections && m_connections != NULL)
		{

			m_connections->bindSilhouettes(m_camera);
            int previousEnd = m_neurons->count() +1;
            int endOfItems = m_neurons->count() + m_connections->count();
            for (QVector<Range>::iterator it = m_ranges.begin() ; it != m_ranges.end(); it++)
            {
                Range range = *it;
                if (range.end <= m_neurons->count())
                    continue;
                else if (range.start > previousEnd)
                    m_connections->drawSubset(previousEnd-1- m_neurons->count(),range.start-previousEnd);
                previousEnd = range.end;
            }
            if (previousEnd < endOfItems)
                m_connections->drawSubset(previousEnd-1- m_neurons->count(),endOfItems-previousEnd);

			m_connections->releaseSilhouettes();

		}
		m_maps["id"].release();


    }
	
    m_maps["diffuse"].release();
    m_maps["depth"].release();

    m_frameBufferObject.enableColorAttachments(0);
    m_frameBufferObject.release();




}

void NCVCanvas::paintEvent(QPaintEvent *e)
{

    if (!isValid())
        return;
    Q_UNUSED(e);

    if (m_neurons != NULL)
    {
        m_renderDirty |= m_neurons->dirty();
        m_neurons->resolve();
    }
    if (m_connections != NULL)
    {
        m_renderDirty |= m_connections->dirty();
        m_connections->resolve();
    }
    if (m_renderDirty)
    {
        if (m_ranges.count() >0)
            m_performSelectionRender();
        else
            m_performRegularRender();
        m_renderDirty = false;
    }
    m_frameBufferObject.blitTexture(m_maps["diffuse"],QGLXTexture::Color0,QRect(0,0,m_width,m_height),QRect(0,0,m_width,m_height));


    glDisable(GL_DEPTH_TEST);
    m_painter.begin(this);
    QPen pen = m_painter.pen();
    pen.setColor(QColor(0,255,0));
    pen.setStyle(Qt::DotLine);
    m_painter.setPen(pen);
    QBrush brush = m_painter.brush();
    brush.setColor(QColor(125,0,125));
    brush.setStyle(Qt::Dense4Pattern);
    m_painter.setBrush(brush);

    if (m_leftMouseDown)
        m_painter.drawRect(m_selectionRect);

    if (m_neurons != NULL && m_connections != NULL && m_idleTimer.elapsed() >= m_legendShowTime)
        m_drawLegend();

    m_painter.end();
    glEnable(GL_DEPTH_TEST);


    
    this->swapBuffers();
    frameRendered();

}



void NCVCanvas::leaveEvent(QEvent * )
{
    if (!isValid())
        return;
    this->parentWidget()->setFocus();
}

void NCVCanvas::enterEvent(QEvent * )
{
    if (!isValid())
        return;
    setFocus();
}



void NCVCanvas::mouseMoveEvent(QMouseEvent* e)
{

    m_idleTimer.restart();
    QVector2D newPos = QVector2D(e->x(),e->y());

    // if left mouse button down, expand selection rectangle
    if (m_leftMouseDown)
    {
        int newWidth = newPos.x() - m_selectionRect.x();
        int newHeight = newPos.y() - m_selectionRect.y();
        if (newWidth != 0)
            m_selectionRect.setWidth(newWidth);
        else
            m_selectionRect.setWidth(1);

        if (newHeight != 0)
            m_selectionRect.setHeight(newHeight);
        else
            m_selectionRect.setHeight(1);
    }
    // if right mouse button down, rotate camera
    else if (m_rightMouseDown)
    {
        m_renderDirty = true;

        QVector2D mouseMovementToExecute = newPos- m_mousePosition;
        m_camera.rotate(mouseMovementToExecute.x() * -m_turnSpeed,mouseMovementToExecute.y() * -m_turnSpeed);
        cameraUpdated(m_camera);
    }
    m_mousePosition = newPos;
}


void NCVCanvas::mousePressEvent(QMouseEvent* e)
{
    m_mousePosition = QVector2D(e->x(),e->y());
    // if left button pressed, start building selection rect.
    if (e->button() == Qt::LeftButton)
    {
        m_leftMouseDown = true;
        m_selectionRect = QRect(e->x(),e->y(),1,1);
    }


    else if (e->button() == Qt::RightButton)
        m_rightMouseDown = true;

}



void NCVCanvas::setSelection(QVector<Range> selection, SelectionFlag flags)
{
	if (selection != m_ranges || flags != m_selectionFlags)
	{
		m_ranges = selection;
        m_selectionFlags = flags;
		selectionChanged(m_ranges,m_selectionFlags);
		m_renderDirty = true;
	}
}

void  NCVCanvas::mouseReleaseEvent(QMouseEvent* e)
{

    if (!isValid())
        return;
    if (e->button() == Qt::LeftButton)
	{
		m_renderDirty = true;
        // correct selection rectangles
        int width = qAbs(m_selectionRect.width());
        int height = qAbs(m_selectionRect.height());

        m_selectionRect.setLeft(qMax(0,qMin(m_selectionRect.left(),m_selectionRect.right())));
        m_selectionRect.setTop(qMax(0,qMin(m_selectionRect.top(),m_selectionRect.bottom())));

        if (m_selectionRect.bottom() > m_height)
            m_selectionRect.setHeight(m_height - m_selectionRect.top());
        else
            m_selectionRect.setHeight(height);

        if (m_selectionRect.right() > m_width)
            m_selectionRect.setWidth(m_width - m_selectionRect.left());
        else
            m_selectionRect.setWidth(width);


        // get pixels of ID map in area of selection window
        GLuint * objectID;
        objectID = new GLuint[ m_selectionRect.width() *  m_selectionRect.height()];
        m_frameBufferObject.getTextureData(m_maps["id"],m_selectionRect,objectID);

        // if not performing compound selection, deselect previously selected items
        if (((int)m_selectionFlags & (int)CompoundSelection ) == 0)
            m_selectedObjects.clear();

        // compile new selections into a unique set
        for (int i =0; i < m_selectionRect.width() *  m_selectionRect.height(); i ++)
        {
            if (objectID[i] != 0 )
            {
                GLuint id = objectID[i];
                m_selectedObjects.insert(id);
            }
        }


        m_ranges.clear();
        if (m_selectedObjects.count() > 0)
        {
            QList<GLuint> list = m_selectedObjects.toList();
            qSort(list);
            Range r = Range();
            r.start = list[0];
            r.end = list[0]+1;
            m_ranges.push_back(r);
            int currentRange = 0;
            for (QList<GLuint>::iterator it = list.begin() +1; it != list.end(); it++)
            {
                int id = *it;
                if (m_ranges[currentRange].end == id)
                    m_ranges[currentRange].end = id+1;
                else
                {
                    Range r = Range();
                    r.start = id;
                    r.end = id+1;
                    m_ranges.push_back(r);
                    currentRange ++;
                }

            }


        }
		
		selectionChanged(m_ranges,m_selectionFlags);

        m_leftMouseDown = false;

    }
    else if (e->button() == Qt::RightButton)
    {
        m_rightMouseDown = false;
    }
}


void NCVCanvas::keyPressEvent( QKeyEvent* e )
{

    switch ( e->key() )
    {

    case Qt::Key_W:
        m_camera.pan(0.0f,0.0f,m_moveSpeed);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;

    case Qt::Key_S:
        m_camera.pan(0.0f,0.0f,-m_moveSpeed);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;

    case Qt::Key_A:
        m_camera.pan(-m_moveSpeed,0.0f,0.0f);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;

    case Qt::Key_D:
        m_camera.pan(m_moveSpeed,0.0f,0.0f);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;


    case Qt::Key_R:
        m_camera.pan(0.0f,0.0f,m_moveSpeed/25.0f);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;



    case Qt::Key_E:
        m_camera.pan(0.0f,m_moveSpeed,0.0f);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;

    case Qt::Key_C:
        m_camera.pan(0.0f,-m_moveSpeed,0.0f);
        m_renderDirty = true;
        cameraUpdated(m_camera);
        break;

    default:
        QGLWidget::keyPressEvent( e );
    }

}

void NCVCanvas::renderNeurons(bool render)
{
    m_renderNeurons = render;
    m_renderDirty = true;
}

void NCVCanvas::renderConections(bool render)
{
    m_renderConnections = render;
    m_renderDirty = true;
}
void NCVCanvas::setNeurons(NCVNeuronSet * neurons)
{
    m_neurons = neurons;
    if (m_neurons != NULL)
    {
        m_worldSize = neurons->bounds().size();
        m_worldCenter = neurons->bounds().center();
        m_moveSpeed = m_worldSize.x()/50;

        m_camera.setView(QVector3D(-m_worldSize.x(),m_worldSize.y(),-m_worldSize.z()), QVector3D(0,0,1),QVector3D(0,1,0));
        m_camera.setProjection(35.0, 4.0/3.0, 0.1f, m_worldSize.length()*2.0f );
    }
    m_renderDirty = true;
}

void NCVCanvas::setConnections(NCVConnectionSet * connections)
{
    m_connections = connections;
    m_renderDirty = true;
}


