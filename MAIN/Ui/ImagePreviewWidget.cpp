#include "ImagePreviewWidget.hpp"

#include <QLabel>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QSizePolicy>

ImagePreviewWidget::ImagePreviewWidget(QWidget* parent) : QWidget(parent), m_image_label{new QLabel{this}} {
	m_layout = new QVBoxLayout{this};
	setLayout(m_layout);
	m_layout->addWidget(m_image_label);
	m_layout->setAlignment(m_image_label, Qt::AlignCenter);
	// m_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_image_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

void ImagePreviewWidget::set_image(const QImage& image) {
	m_image_pixmap = QPixmap::fromImage(image);
	m_image_label->setPixmap(m_image_pixmap.scaledToHeight(200));
	adjustSize();
	// resize(m_image_label->pixmap()->size());
}

void ImagePreviewWidget::clear_image() { m_image_label->clear(); }

void ImagePreviewWidget::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	if (m_image_pixmap.isNull()) { return; }
	m_image_label->setPixmap(m_image_pixmap.scaled(event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

	// m_image_label->setFixedSize(m_image_label->pixmap()->size());
}
