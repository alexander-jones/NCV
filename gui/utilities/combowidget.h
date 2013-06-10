#ifndef SIDEBAR_H
#define SIDEBAR_H


#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QSignalMapper>
#include <QSpinBox>
#include <QComboBox>
#include <QHBoxLayout>

/*!
    \class ComboWidget
    \author Alex Jones
    \brief A widget class for cycling through a number of widgets through a combo box.
*/
class ComboWidget : public QWidget
{
    Q_OBJECT
public:
	
	/*!
		\param parent A pointer to the parent widget.
		\brief Constructs a ComboWidget
	*/
    explicit ComboWidget(QWidget *parent = 0);
	
	/*!
		\param tool A tool to added to the ComboWidget toolbar.
		\brief Adds a tool the to the toolbar which always contains the combo box switcher.
	*/
    void addTool(QWidget * tool);


    /*!
        \param index The index in the toolbar to insert the new tool.
        \param tool A tool to added to the ComboWidget toolbar.
        \brief Inserts a tool the to the toolbar which always contains the combo box switcher.
    */
    void insertTool(int index,QWidget * tool);
	
	/*!
		\param tool The tool to remove from the ComboWidget toolbar.
		\brief Removes a tool the from the toolbar.
	*/
    void removeTool(QWidget * tool);
	

	/*!
		\param widget The widget to add.
		\param key The key for the widget that will appear in the combo box.
		\brief Adds a widget reffered to with key to the list of widgets.
	*/
    void addWidget(QWidget * widget,const QString& key);
	
	/*!
		\param key The key that appears in the combo box for the widget .
		\brief Removes a widget reffered to with key from the list of widgets.
	*/
    void removeWidget(QString key);
	
	/*!
		\brief Returns the current number of widgets contained in the list of widgets.
	*/
    int count();
	
	/*!
		\param key The key that appears in the combo box for the widget .
		\brief Returns whether or not the widget reffered to with key is present in the list of widgets.
	*/
    bool containsWidget(QString key);
	
	/*!
		\param key The tool to check for.
		\brief Returns whether or not the tool is contained in the toolbar alongside the combo widget.  
	*/
    bool containsTool(QWidget * tool);
	
	/*!
		\brief Returns the current widget. If no widget has been added, returns the void widget pointer.
	*/
    QWidget * currentWidget();
		
	/*!
		\brief Returns the widget presented when no widgets are in the list of widgets.
	*/
    QWidget * voidWidget();

	/*!
		\brief Returns the key of the current widget. Returns "" if no widgets have been added. 
	*/
    QString currentWidgetKey();

	/*!
		\param widget The widget to use.
		\brief Sets' widget' to be presented when no widgets have been added.
	*/
    void setVoidWidget(QWidget * widget);

signals:
	
	/*!
		\param key The key of the new current widget.
		\brief This signal emits when the current widget has changed.
	*/
    void widgetChanged(QString key);

	
	/*!
		\param widget The new current widget.
		\brief This signal emits when the current widget has changed.
	*/
    void widgetChanged(QWidget * widget);

public slots:
	/*!
		\param key The key of the widget to set as current.
		\brief This slot lets the current widget be set to be the widget reffered to by key.
	*/
    void setWidget( QString key);
	
	/*!
		\param widget The widget to set as current
		\brief This slot lets the current widget be set to be the widget 'widget'.
	*/
    void setWidget( QWidget * widget);

private:
    QWidget * m_currentWidget, *m_toolbar;
    QComboBox * m_widgetSelector;
    QHBoxLayout * m_toolbarLayout;
    QVBoxLayout * m_layout;
    QWidget * m_voidWidget;
    QFrame *m_separator;
    QMap<QString, QWidget *> m_widgets;
};

#endif // SIDEBAR_H
