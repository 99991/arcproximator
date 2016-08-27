#include <QApplication>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QDir>
#include <QDebug>

// In your .pro file:
// QT += svg

int main(int argc, char **argv)
{
    // A QApplication instance is necessary if fonts are used in the SVG
    QApplication app(argc, argv);

    // Load your SVG
    QSvgRenderer renderer(QString("/home/karl/svg_logo.svg"));

    // Prepare a QImage with desired characteritisc
    QImage image(500, 200, QImage::Format_ARGB32);
    image.fill(0xaaA08080);  // partly transparent red-ish background

    // Get QPainter that paints to the image
    QPainter painter(&image);
    renderer.render(&painter);

    // Save, image format based on file extension
    image.save("/home/karl/svg_logo.png");

    qDebug() << QDir::currentPath();

    return 0;
}
