#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "appconfig.hpp"
#include "poly_file_manager.hpp"
#include "thumbnailer.hpp"

#include <QMainWindow>
#include <optional>
#include <set>
#include <string>
#include <vector>

class QAction;
class QComboBox;
class QCloseEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;
class QEvent;
class QActionGroup;
class QKeyEvent;
class QLabel;
class QImage;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPoint;
class QPushButton;
class QSplitter;

/**
 * @brief Qt Widgets front-end mirroring the command-line capabilities.
 *
 * Left column lists tags; clicking (or Ctrl+clicking) tags selects them and
 * refreshes the file list on the right. A long-press on a tag reveals a
 * checkbox mode for a persistent multi-selection. Selecting files on the right
 * re-orders the left list so the matching tags move to the top and are marked.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);

  protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

  private slots:
    // Repository lifecycle.
    void openRepository();
    void createRepository();
    void destroyRepository();
    void manageRepositories();

    // Tag CRUD.
    void addTag();
    void renameTag();
    void removeTag();

    // Path CRUD.
    void addPath();
    void replacePath();
    void removePath();
    void removeSelectedPaths(bool confirm);
    void importFromFolder();

    // Associations.
    void editFileTags();

    // Maintenance.
    void checkConsistency();
    void syncToRepo();
    void syncFromRepo();
    void openSettings();
    void undo();

    // Selection helpers.
    void clearSelection();
    void finishCheckboxMode();

    // List behaviour.
    void onTagSearchChanged(const QString &text);
    void onFileSearchChanged(const QString &text);
    void onModeChanged(int index);
    void onTagSelectionChanged();
    void onFileSelectionChanged();
    void onTagItemChanged(QListWidgetItem *item);
    void onFileDoubleClicked(QListWidgetItem *item);
    void onThumbnailReady(const QString &path, int size, const QImage &image);
    void revealSelectedPath();
    void copySelectedPath();
    void onTagContextMenu(const QPoint &pos);
    void onFileContextMenu(const QPoint &pos);

  private:
    void buildUi();
    bool loadRepository(const std::filesystem::path &repo);
    void refreshTagList();
    void refreshFileList();
    void refreshSelectionState();
    void enterCheckboxMode();
    bool commit();
    void applySettings();
    void restoreWindowState();
    void restoreDisplayState();
    void setViewSize(int size);
    void cycleViewSize(int dir);
    void syncViewMenu();
    void applyThumbnailSettings();
    void applyUndoSettings();
    void pushUndo(const FileTagData &snapshot);
    void updateUndoAction();

    std::string selectedTag() const;
    std::vector<std::filesystem::path> selectedPaths() const;
    bool requireManager(const QString &message) const;
    std::vector<std::filesystem::path> collectFolderPaths(const std::filesystem::path &folder,
                                                          bool includeFolder, bool recursive) const;
    void addTagFromPlaceholder(QListWidgetItem *item);

    std::set<FilePath> paths_for_tags(const std::vector<FileTag> &tags) const;
    std::set<FileTag> tags_for_paths(const std::vector<FilePath> &paths) const;

    std::optional<PolyFileManager> manager_;
    AppConfig config_;
    Thumbnailer thumbnailer_;
    std::vector<FileTagData> undo_stack_;
    int max_undo_ = 50;
    QAction *undo_action_ = nullptr;
    std::set<FileTag> active_tags_;    // tags that currently drive the file list
    std::set<FileTag> highlight_tags_; // tags matching the selected file(s), shown on top & marked
    bool checkbox_mode_ = false;
    bool show_name_only_ = false;
    int view_size_ = 16;
    QActionGroup *view_group_ = nullptr;
    std::string settings_view_before_;

    // Widgets.
    QLineEdit *tag_search_ = nullptr;
    QListWidget *tag_list_ = nullptr;
    QComboBox *mode_combo_ = nullptr;
    QLineEdit *file_search_ = nullptr;
    QListWidget *file_list_ = nullptr;
    QPushButton *clear_btn_ = nullptr;
    QLabel *status_label_ = nullptr;
    QLabel *repo_indicator_ = nullptr;
    QSplitter *splitter_ = nullptr;
};

#endif // MAINWINDOW_HPP
