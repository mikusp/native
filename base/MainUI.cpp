#include "MainUI.h"

#include <QResizeEvent>

#include "base/NativeApp.h"
#include "base/display.h"
#include "base/timeutil.h"

void SimulateGamepad(InputState *input) {
	input->pad_lstick_x = 0;
	input->pad_lstick_y = 0;
	input->pad_rstick_x = 0;
	input->pad_rstick_y = 0;

	if (input->pad_buttons & PAD_BUTTON_JOY_UP)
		input->pad_lstick_y=1;
	else if (input->pad_buttons & PAD_BUTTON_JOY_DOWN)
		input->pad_lstick_y=-1;
	if (input->pad_buttons & PAD_BUTTON_JOY_LEFT)
		input->pad_lstick_x=-1;
	else if (input->pad_buttons & PAD_BUTTON_JOY_RIGHT)
		input->pad_lstick_x=1;
}

MainUI::MainUI(float scale, QWidget *parent) :
    QGLWidget(parent), timer(new QTimer()) {
    init(scale);
}

MainUI::MainUI(QWidget *parent) :
    QGLWidget(parent), timer(new QTimer()) {
    init(1);
}

MainUI::~MainUI() {
#ifdef USING_GLES2
    delete acc;
#endif
    NativeShutdownGraphics();
}

void MainUI::init(float scale) {
    dpi_scale = scale;
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_LockLandscapeOrientation);
    pad_buttons = 0;
#ifdef USING_GLES2
    acc = new QAccelerometer(this);
    acc->start();
#endif
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(16);
}

void MainUI::resizeEvent(QResizeEvent *e) {
    pixel_xres = e->size().width();
    pixel_yres = e->size().height();
    dp_xres = pixel_xres * dpi_scale;
    dp_yres = pixel_yres * dpi_scale;
}

bool MainUI::event(QEvent *e) {
    QList<QTouchEvent::TouchPoint> touchPoints;
	switch(e->type())
	{
	case QEvent::TouchBegin:
	case QEvent::TouchUpdate:
	case QEvent::TouchEnd:
		touchPoints = static_cast<QTouchEvent *>(e)->touchPoints();
		foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
			switch (touchPoint.state()) {
			case Qt::TouchPointStationary:
				break;
			case Qt::TouchPointPressed:
			case Qt::TouchPointReleased:
				input_state.pointer_down[touchPoint.id()] = (touchPoint.state() == Qt::TouchPointPressed);
			case Qt::TouchPointMoved:
				input_state.pointer_x[touchPoint.id()] = touchPoint.pos().x() * dpi_scale;
				input_state.pointer_y[touchPoint.id()] = touchPoint.pos().y() * dpi_scale;
				break;
			default:
				break;
			}
		}
	break;
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		input_state.pointer_down[0] = (e->type() == QEvent::MouseButtonPress);
	case QEvent::MouseMove:
		input_state.pointer_x[0] = ((QMouseEvent*)e)->pos().x() * dpi_scale;
		input_state.pointer_y[0] = ((QMouseEvent*)e)->pos().y() * dpi_scale;
	break;
	case QEvent::KeyPress:
		for (int b = 0; b < 18; b++) {
			if (((QKeyEvent*)e)->key() == buttonMappings[b])
				pad_buttons |= (1<<b);
		}
	break;
	case QEvent::KeyRelease:
		for (int b = 0; b < 18; b++) {
			if (((QKeyEvent*)e)->key() == buttonMappings[b])
				pad_buttons &= ~(1<<b);
		}
	break;
	default:
		return QWidget::event(e);
	}
	e->accept();
	return true;
}

void MainUI::initializeGL() {
#ifndef USING_GLES2
    glewInit();
#endif
    NativeInitGraphics();
}

void MainUI::paintGL() {
    input_state.pad_buttons = pad_buttons;
	SimulateGamepad(&input_state);
	updateAccelerometer();
	UpdateInputState(&input_state);
	NativeUpdate(input_state);
	EndInputState(&input_state);
	NativeRender();
	time_update();
}

void MainUI::updateAccelerometer() {
#ifdef USING_GLES2
	// TODO: Toggle it depending on whether it is enabled
	QAccelerometerReading *reading = acc->reading();
	if (reading) {
		input_state.acc.x = reading->x();
		input_state.acc.y = reading->y();
		input_state.acc.z = reading->z();
	}
#endif
}
