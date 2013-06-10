/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** orientationbutton.cpp
** This is an extended version of Qt's QPushButton. In addition to
** QPushButton's functionality, this button's orientation can be changed
** to vertical and the button can also be mirrored.
**
** Author:	J-P Nurmi
** Date:	23th September 2006
** -------------------------------------------------------------------------*/
#include "orientationbutton.h"
#include <QStylePainter>
#include <QMenu>

OrientationButton::OrientationButton(QWidget* parent)
	: QPushButton(parent)
{
	init();
}

OrientationButton::OrientationButton(const QString& text, QWidget* parent)
	: QPushButton(text, parent)
{
	init();
}

OrientationButton::OrientationButton(const QIcon& icon, const QString& text, QWidget* parent)
	: QPushButton(icon, text, parent)
{
	init();
}

OrientationButton::~OrientationButton()
{
}

void OrientationButton::init()
{
	orientation_ = Qt::Horizontal;
	mirrored_ = false;
}

Qt::Orientation OrientationButton::orientation() const
{
	return orientation_;
}

void OrientationButton::setOrientation(Qt::Orientation orientation)
{
	orientation_ = orientation;
	switch (orientation)
	{
		case Qt::Horizontal:
			setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
			break;
		
		case Qt::Vertical:
			setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
			break;
	}
}

bool OrientationButton::mirrored() const
{
	return mirrored_;
}

void OrientationButton::setMirrored(bool mirrored)
{
	mirrored_ = mirrored;
}

QSize OrientationButton::sizeHint() const
{
	QSize size = QPushButton::sizeHint();
	if (orientation_ == Qt::Vertical)
		size.transpose();
	return size;
}

QSize OrientationButton::minimumSizeHint() const
{
	QSize size = QPushButton::minimumSizeHint();
	if (orientation_ == Qt::Vertical)
		size.transpose();
	return size;
}

void OrientationButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QStylePainter p(this);

	switch (orientation_)
	{
		case Qt::Horizontal:
			if (mirrored_)
			{
				p.rotate(180);
				p.translate(-width(), -height());
			}
			break;

		case Qt::Vertical:
			if (mirrored_)
			{
				p.rotate(-90);
				p.translate(-height(), 0);
			}
			else
			{
				p.rotate(90);
				p.translate(0, -width());
			}
			break;
	}

	p.drawControl(QStyle::CE_PushButton, getStyleOption());
}

QStyleOptionButton OrientationButton::getStyleOption() const
{
	QStyleOptionButton opt;
	opt.initFrom(this);
	if (orientation_ == Qt::Vertical)
	{
		QSize size = opt.rect.size();
		size.transpose();
		opt.rect.setSize(size);
	}
	opt.features = QStyleOptionButton::None;
	if (isFlat())
		opt.features |= QStyleOptionButton::Flat;
	if (menu())
		opt.features |= QStyleOptionButton::HasMenu;
	if (autoDefault() || isDefault())
		opt.features |= QStyleOptionButton::AutoDefaultButton;
	if (isDefault())
		opt.features |= QStyleOptionButton::DefaultButton;
	if (isDown() || (menu() && menu()->isVisible()))
		opt.state |= QStyle::State_Sunken;
	if (isChecked())
		opt.state |= QStyle::State_On;
	if (!isFlat() && !isDown())
		opt.state |= QStyle::State_Raised;
	opt.text = text();
	opt.icon = icon();
	opt.iconSize = iconSize();
	return opt;
}
