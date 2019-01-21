#pragma once

#include <QLabel>
#include <QPixmap>
#include <QStylePainter>
#include <QWidget>

class ImagePreviewWidget : public QLabel {
Q_OBJECT

public:
	ImagePreviewWidget(QWidget * parent = nullptr);

	void set_image(const QImage & image);

	void clear_image();

	void paintEvent(QPaintEvent * aEvent);

	void setPixmap(QPixmap aPicture);

protected:
	QSize minimumSizeHint() const override;

private:
	void _display_image();

	QPixmap m_source_pixmap;
	QPixmap m_current_pixmap;
};
