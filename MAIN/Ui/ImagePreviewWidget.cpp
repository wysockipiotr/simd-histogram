#include "ImagePreviewWidget.hpp"
#include <QApplication>

ImagePreviewWidget::ImagePreviewWidget(QWidget * parent)
	: QLabel(parent) {
}

void ImagePreviewWidget::set_image(const QImage & image) {
	m_source_pixmap = QPixmap::fromImage(image);
	setPixmap(m_source_pixmap);
}

void ImagePreviewWidget::clear_image() {
	clear();
}

void ImagePreviewWidget::paintEvent(QPaintEvent * aEvent) {
	QLabel::paintEvent(aEvent);
	_display_image();
}

void ImagePreviewWidget::setPixmap(QPixmap aPicture) {
	m_source_pixmap = m_current_pixmap = aPicture;
	repaint();
}

QSize ImagePreviewWidget::minimumSizeHint() const {
	return { 300, 200 };
}

void ImagePreviewWidget::_display_image() {

	if (m_source_pixmap.isNull()) {
		return;
	}

	const auto cw { static_cast<float>(width()) };
	const auto ch { static_cast<float>(height()) };
	const auto pw { static_cast<float>(m_current_pixmap.width()) };
	const auto ph { static_cast<float>(m_current_pixmap.height()) };

	if (pw > cw && ph > ch &&
	    pw / cw > ph / ch || 
	    pw > cw && ph <= ch || 
	    pw < cw && ph < ch &&
	    cw / pw < ch / ph 
	) {
		m_current_pixmap = m_source_pixmap.scaledToWidth(cw, Qt::TransformationMode::FastTransformation);
	} else if (pw > cw && ph > ch &&
	           pw / cw <= ph / ch ||
	           ph > ch && pw <= cw || 
	           pw < cw && ph < ch &&
	           cw / pw > ch / ph 
	) {
		m_current_pixmap = m_source_pixmap.scaledToHeight(ch, Qt::TransformationMode::FastTransformation);
	}

	const auto x { static_cast<int>(( cw - m_current_pixmap.width() ) / 2) };
	const auto y { static_cast<int>(( ch - m_current_pixmap.height() ) / 2) };

	QPainter paint(this);
	paint.drawPixmap(x, y, m_current_pixmap);
}
