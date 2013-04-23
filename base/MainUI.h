#ifndef MAINUI_H
#define MAINUI_H

#include "gfx_es2/glsl_program.h"
#include <QGLWidget>
#include <QTimer>

#include "input/input_state.h"

const int buttonMappings[18] = {
	Qt::Key_X + 0x20,   //A
	Qt::Key_S + 0x20,   //B
	Qt::Key_Z + 0x20,   //X
	Qt::Key_A + 0x20,   //Y
	Qt::Key_Q + 0x20,   //LBUMPER
	Qt::Key_W + 0x20,   //RBUMPER
	Qt::Key_1,          //START
	Qt::Key_2,          //SELECT
	Qt::Key_Up,         //UP
	Qt::Key_Down,       //DOWN
	Qt::Key_Left,       //LEFT
	Qt::Key_Right,      //RIGHT
	0,                  //MENU (event)
	Qt::Key_Backspace,  //BACK
	Qt::Key_I + 0x20,   //JOY UP
	Qt::Key_K + 0x20,   //JOY DOWN
	Qt::Key_J + 0x20,   //JOY LEFT
	Qt::Key_L + 0x20,   //JOY RIGHT
};

void SimulateGamepad(InputState *input);

class MainUI : public QGLWidget {
    Q_OBJECT
public:
    explicit MainUI(float scale, QWidget *parent = 0);
    explicit MainUI(QWidget *parent = 0);
    ~MainUI();

protected:
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent *e);
    void initializeGL();
    void paintGL();
    void updateAccelerometer();

private:
    void init(float scale = 1);
    int pad_buttons;
    InputState input_state;
#ifdef USING_GLES2
    QAccelerometer *acc;
#endif
    float dpi_scale;
    QTimer* timer;
};

#endif
