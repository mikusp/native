#ifndef QTMAIN_H
#define QTMAIN_H

#include <QTouchEvent>
#include <QMouseEvent>
#include "gfx_es2/glsl_program.h"
#include <QGLWidget>

#include <QAudioOutput>
#include <QAudioFormat>
#ifdef USING_GLES2
#include <QAccelerometer>
QTM_USE_NAMESPACE
#endif

#include "base/logging.h"
#include "base/NativeApp.h"
#include "net/resolve.h"
#include "display.h"

// Audio
#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_SAMPLES 1024
#define AUDIO_SAMPLESIZE 16
class MainAudio: public QObject
{
	Q_OBJECT
public:
	MainAudio() {
		QAudioFormat fmt;
		fmt.setFrequency(AUDIO_FREQ);
		fmt.setCodec("audio/pcm");
		fmt.setChannelCount(AUDIO_CHANNELS);
		fmt.setSampleSize(AUDIO_SAMPLESIZE);
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::SignedInt);
		mixlen = 2*AUDIO_CHANNELS*AUDIO_SAMPLES;
		mixbuf = (char*)malloc(mixlen);
		output = new QAudioOutput(fmt);
		output->setBufferSize(mixlen);
		feed = output->start();
		timer = startTimer(1000*AUDIO_SAMPLES / AUDIO_FREQ);
	}
	~MainAudio() {
		killTimer(timer);
		feed->close();
		output->stop();
		delete output;
		free(mixbuf);
	}

protected:
	void timerEvent(QTimerEvent *) {
		memset(mixbuf, 0, mixlen);
		NativeMix((short *)mixbuf, mixlen / 4);
		feed->write(mixbuf, mixlen);
	}
private:
	QIODevice* feed;
	QAudioOutput* output;
	int mixlen;
	char* mixbuf;
	int timer;
};

#endif

