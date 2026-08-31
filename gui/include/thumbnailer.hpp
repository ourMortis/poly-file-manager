#ifndef THUMBNAILER_HPP
#define THUMBNAILER_HPP

#include <QCache>
#include <QIcon>
#include <QImage>
#include <QObject>
#include <QString>
#include <QThreadPool>

/**
 * @brief Asynchronously generates and caches image thumbnails.
 *
 * Decoding runs on a QThreadPool so the UI never blocks; the resulting QImage is
 * delivered back to the GUI thread through the queued thumbnailReady() signal,
 * where it is converted into a QIcon and cached. Minimum thumbnail size and the
 * cache budget (in MB) are configurable.
 */
class Thumbnailer : public QObject
{
    Q_OBJECT

  public:
    explicit Thumbnailer(QObject *parent = nullptr);
    ~Thumbnailer() override;

    void setMinSize(int size);
    void setCacheMb(int mb);

    [[nodiscard]] bool canThumbnail(const QString &path, int size) const;
    [[nodiscard]] QIcon cached(const QString &path, int size) const;
    void store(const QString &path, int size, const QIcon &icon);
    void request(const QString &path, int size);

  signals:
    void thumbnailReady(const QString &path, int size, const QImage &image);

  private:
    QThreadPool pool_;
    QCache<QString, QIcon> cache_;
    int min_size_ = 48;
};

#endif // THUMBNAILER_HPP
