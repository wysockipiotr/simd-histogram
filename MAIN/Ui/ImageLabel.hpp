#pragma once
#include <QWidget>
#include <QStylePainter>
#include <QResizeEvent>

class ImageLabel : public QWidget {
public:
	ImageLabel(QWidget* parent = nullptr) : QWidget(parent) {
		setAutoFillBackground(true);
		setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	}

	void set_image(const QImage& image) { m_pixmap = QPixmap::fromImage(image).scaledToHeight(250); }


protected:
	QSize minimumSizeHint() const override { return {200, 200}; }
	QSize sizeHint() const override { return m_resized; }

	void resizeEvent(QResizeEvent* event) override {
		QWidget::resizeEvent(event);
		m_resized = event->size();
		update();
	}

	void paintEvent(QPaintEvent* event) override {
		QStylePainter painter{this};
		painter.drawPixmap(0, 0, m_pixmap.scaled(m_resized, Qt::KeepAspectRatio));
	}

private:
	QPixmap m_pixmap;
	QSize m_resized;
};
