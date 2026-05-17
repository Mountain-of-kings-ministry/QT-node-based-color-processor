#pragma once

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QVector>
#include <QApplication>
#include <QScreen>

class PreviewWindow : public QWidget {
    Q_OBJECT
public:
    explicit PreviewWindow(QWidget *parent = nullptr);
    ~PreviewWindow() override = default;

    void showImage(const QImage &image, const QString &colorSpace);

    void setDisplayOptions(const QStringList &displays);
    void setViewOptions(const QStringList &views);

signals:
    void displayChanged(QString display);
    void viewChanged(QString view);
    void exposureChanged(double stops);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void updateTransform();

    QImage m_image;
    QLabel *m_infoLabel = nullptr;
    QComboBox *m_displayCombo = nullptr;
    QComboBox *m_viewCombo = nullptr;
    QSlider *m_exposureSlider = nullptr;

    double m_zoom = 1.0;
    double m_panX = 0.0, m_panY = 0.0;
    bool m_dragging = false;
    QPoint m_lastMousePos;

    QString m_colorSpace;
};
