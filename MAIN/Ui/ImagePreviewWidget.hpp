#pragma once

#include <QWidget>
#include <QDockWidget>

class ImagePreviewWidget : public QWidget {
Q_OBJECT

public:
	ImagePreviewWidget(QWidget* parent = nullptr);

public slots:
	void set_image(const QImage& image);

protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	QPixmap m_image_pixmap;
	class QLabel* m_image_label{};
	class QVBoxLayout* m_layout{};
};
