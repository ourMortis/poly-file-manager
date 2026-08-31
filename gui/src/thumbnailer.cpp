#include "thumbnailer.hpp"

#include <QEventLoop>
#include <QFileInfo>
#include <QImageReader>
#ifdef HAVE_QT_MULTIMEDIA
#include <QMediaPlayer>
#endif
#include <QPainter>
#include <QPixmap>
#include <QRunnable>
#include <QStringList>
#ifdef HAVE_QT_MULTIMEDIA
#include <QTimer>
#include <QUrl>
#include <QVideoFrame>
#include <QVideoSink>
#endif

namespace
{

bool isVideoExtension(const QString &suffix)
{
    static const QStringList exts = {QStringLiteral("mp4"), QStringLiteral("mkv"), QStringLiteral("avi"),
                                     QStringLiteral("mov"), QStringLiteral("wmv"), QStringLiteral("flv"),
                                     QStringLiteral("webm"), QStringLiteral("m4v"), QStringLiteral("mpg"),
                                     QStringLiteral("mpeg"), QStringLiteral("3gp"), QStringLiteral("ts"),
                                     QStringLiteral("rmvb"), QStringLiteral("rm")};
    return exts.contains(suffix);
}

QImage loadScaledImage(const QString &path, int size)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);
    reader.setScaledSize(QSize(size, size));
    return reader.read();
}

QImage centerSquare(const QImage &image, int size)
{
    if (image.isNull())
    {
        return {};
    }
    QImage out(size, size, QImage::Format_ARGB32_Premultiplied);
    out.fill(Qt::transparent);
    QPainter painter(&out);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    const QImage scaled = image.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter.drawImage((size - scaled.width()) / 2, (size - scaled.height()) / 2, scaled);
    painter.end();
    return out;
}

#ifdef HAVE_QT_MULTIMEDIA
// Grab one video frame (the poster/first frame) via Qt Multimedia.
QImage grabVideoFrame(const QString &path)
{
    QMediaPlayer player;
    QVideoSink sink;
    player.setVideoSink(&sink);

    QImage frame;
    QEventLoop loop;
    bool got = false;
    QObject::connect(&sink, &QVideoSink::videoFrameChanged, &loop, [&](const QVideoFrame &video) {
        if (video.isValid() && !got)
        {
            got = true;
            frame = video.toImage();
            loop.quit();
        }
    });

    player.setSource(QUrl::fromLocalFile(path));
    player.setPosition(0);
    player.play();
    QTimer::singleShot(3000, &loop, [&] { loop.quit(); });
    loop.exec();
    player.stop();
    return frame;
}
#endif

} // namespace

Thumbnailer::Thumbnailer(QObject *parent) : QObject(parent)
{
    pool_.setMaxThreadCount(4);
    cache_.setMaxCost(16 * 1024 * 1024);
}

Thumbnailer::~Thumbnailer()
{
    pool_.clear();
    pool_.waitForDone();
}

void Thumbnailer::setMinSize(int size)
{
    min_size_ = size < 0 ? 0 : size;
}

void Thumbnailer::setCacheMb(int mb)
{
    cache_.setMaxCost(static_cast<int>(static_cast<qint64>(mb) * 1024 * 1024));
}

bool Thumbnailer::canThumbnail(const QString &path, int size) const
{
    if (size < min_size_)
    {
        return false;
    }
    const QString suffix = QFileInfo(path).suffix().toLower();
    if (isVideoExtension(suffix))
    {
#ifdef HAVE_QT_MULTIMEDIA
        return true;
#else
        return false;
#endif
    }
    return QImageReader::supportedImageFormats().contains(suffix.toLatin1());
}

QIcon Thumbnailer::cached(const QString &path, int size) const
{
    const QString key = path + QStringLiteral("@") + QString::number(size);
    if (const QIcon *icon = cache_.object(key))
    {
        return *icon;
    }
    return {};
}

void Thumbnailer::store(const QString &path, int size, const QIcon &icon)
{
    if (icon.isNull())
    {
        return;
    }
    const QString key = path + QStringLiteral("@") + QString::number(size);
    cache_.insert(key, new QIcon(icon), size * size * 4);
}

void Thumbnailer::request(const QString &path, int size)
{
    if (!canThumbnail(path, size) || !cached(path, size).isNull())
    {
        return;
    }
    const bool video = isVideoExtension(QFileInfo(path).suffix().toLower());
    pool_.start(QRunnable::create([this, path, size, video]() {
        QImage image;
#ifdef HAVE_QT_MULTIMEDIA
        if (video)
        {
            image = grabVideoFrame(path);
        }
        else
#endif
        {
            image = loadScaledImage(path, size);
        }
        image = centerSquare(image, size);
        if (!image.isNull())
        {
            emit thumbnailReady(path, size, image);
        }
    }));
}
