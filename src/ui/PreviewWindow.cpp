#include "PreviewWindow.h"

#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>

PreviewWindow::PreviewWindow(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint)
{
    setWindowTitle("MOKM Preview");
    setMinimumSize(320, 240);
    resize(640, 480);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto *toolbar = new QWidget(this);
    toolbar->setStyleSheet("background-color: #1E293B; padding: 4px;");
    auto *toolLayout = new QHBoxLayout(toolbar);
    toolLayout->setContentsMargins(8, 4, 8, 4);
    toolLayout->setSpacing(8);

    m_displayCombo = new QComboBox(toolbar);
    m_displayCombo->setStyleSheet("color: #F8FAFC; background: #0F172A; border: 1px solid #243041; border-radius: 4px; padding: 2px 6px;");
    m_viewCombo = new QComboBox(toolbar);
    m_viewCombo->setStyleSheet("color: #F8FAFC; background: #0F172A; border: 1px solid #243041; border-radius: 4px; padding: 2px 6px;");

    m_exposureSlider = new QSlider(Qt::Horizontal, toolbar);
    m_exposureSlider->setRange(-500, 500);
    m_exposureSlider->setValue(0);
    m_exposureSlider->setFixedWidth(120);
    m_exposureSlider->setStyleSheet(
        "QSlider::groove:horizontal { height: 4px; background: #334155; border-radius: 2px; }"
        "QSlider::handle:horizontal { background: #FACC15; width: 12px; height: 12px; margin: -4px 0; border-radius: 6px; }"
    );

    auto *exposureLabel = new QLabel("Exp: 0.0", toolbar);
    exposureLabel->setStyleSheet("color: #94A3B8; font-size: 11px;");

    toolLayout->addWidget(new QLabel("Display:", toolbar));
    toolLayout->addWidget(m_displayCombo);
    toolLayout->addWidget(new QLabel("View:", toolbar));
    toolLayout->addWidget(m_viewCombo);
    toolLayout->addWidget(exposureLabel);
    toolLayout->addWidget(m_exposureSlider);
    toolLayout->addStretch();

    m_infoLabel = new QLabel("No image", toolbar);
    m_infoLabel->setStyleSheet("color: #94A3B8; font-size: 11px;");
    toolLayout->addWidget(m_infoLabel);

    layout->addWidget(toolbar);

    setStyleSheet("background-color: #0B1120;");

    connect(m_displayCombo, &QComboBox::currentTextChanged, this, &PreviewWindow::displayChanged);
    connect(m_viewCombo, &QComboBox::currentTextChanged, this, &PreviewWindow::viewChanged);
    connect(m_exposureSlider, &QSlider::valueChanged, this, [this, exposureLabel](int val) {
        double stops = val / 100.0;
        exposureLabel->setText(QString("Exp: %1").arg(stops, 0, 'f', 1));
        emit exposureChanged(stops);
    });
}

void PreviewWindow::showImage(const QImage &image, const QString &colorSpace)
{
    m_image = image;
    m_colorSpace = colorSpace;
    m_infoLabel->setText(QString("%1x%2 | %3").arg(image.width()).arg(image.height()).arg(colorSpace));
    update();
}

void PreviewWindow::setDisplayOptions(const QStringList &displays)
{
    m_displayCombo->clear();
    m_displayCombo->addItems(displays);
}

void PreviewWindow::setViewOptions(const QStringList &views)
{
    m_viewCombo->clear();
    m_viewCombo->addItems(views);
}

void PreviewWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor("#0B1120"));

    if (m_image.isNull()) {
        p.setPen(QColor("#94A3B8"));
        p.drawText(rect(), Qt::AlignCenter, "No image connected");
        return;
    }

    p.save();
    p.translate(width() / 2.0 + m_panX, height() / 2.0 + m_panY - 24);
    p.scale(m_zoom, m_zoom);
    p.translate(-m_image.width() / 2.0, -m_image.height() / 2.0);
    p.drawImage(0, 0, m_image);
    p.restore();
}

void PreviewWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && event->position().y() > 28) {
        m_dragging = true;
        m_lastMousePos = event->pos();
    }
}

void PreviewWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_panX += delta.x();
        m_panY += delta.y();
        m_lastMousePos = event->pos();
        update();
    }
}

void PreviewWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

void PreviewWindow::wheelEvent(QWheelEvent *event)
{
    double factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    m_zoom = std::clamp(m_zoom * factor, 0.05, 20.0);
    update();
}

void PreviewWindow::updateTransform()
{
    update();
}
