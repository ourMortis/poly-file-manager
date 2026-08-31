#ifndef REPOMANAGERDIALOG_HPP
#define REPOMANAGERDIALOG_HPP

#include "appconfig.hpp"

#include <QDialog>
#include <QString>

class QListWidget;

/**
 * @brief Window listing every repository opened before.
 *
 * Shows each known repository with an existence marker, and lets the user open,
 * create, forget (remove from the list) or physically destroy a repository.
 */
class RepoManagerDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit RepoManagerDialog(AppConfig &config, QWidget *parent = nullptr);

  signals:
    void openRequested(const QString &path);

  private slots:
    void onOpen();
    void onCreate();
    void onRemove();
    void onDestroy();
    void refresh();

  private:
    void rebuildList();

    AppConfig &config_;
    QListWidget *list_ = nullptr;
};

#endif // REPOMANAGERDIALOG_HPP
