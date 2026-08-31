#include "mainwindow.hpp"
#include "edittagsdialog.hpp"
#include "importdialog.hpp"
#include "repomanagerdialog.hpp"
#include "settingsdialog.hpp"
#include "tagdelegate.hpp"

#include <QActionGroup>
#include <QAction>
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QFontDatabase>
#include <QFont>
#include <QGuiApplication>
#include <QHash>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QPixmap>
#include <QStatusBar>
#include <QSet>
#include <QSplitter>
#include <QStringList>
#include <QUrl>
#include <QVBoxLayout>
#include <QWheelEvent>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <vector>

namespace
{

constexpr int kAddItemRole = Qt::UserRole + 1;

const std::vector<int> kIconSizes = {16,  20,  24,  32,  40,  48,  56,  64,
                                     80,  96,  112, 128, 160, 192, 224, 256};

QString presetNameForSize(int size)
{
    static const std::vector<std::pair<int, QString>> presets = {
        {16, QStringLiteral("列表")},
        {32, QStringLiteral("小图标")},
        {64, QStringLiteral("中图标")},
        {128, QStringLiteral("大图标")},
    };
    int best_dist = 1 << 30;
    int best_size = presets.front().first;
    QString best_name = presets.front().second;
    for (const auto &[s, name] : presets)
    {
        const int dist = std::abs(size - s);
        if (dist < best_dist)
        {
            best_dist = dist;
            best_size = s;
            best_name = name;
        }
    }
    (void)best_size;
    return best_name;
}

int presetSizeForName(const QString &name)
{
    if (name == QStringLiteral("小图标"))
    {
        return 32;
    }
    if (name == QStringLiteral("中图标"))
    {
        return 64;
    }
    if (name == QStringLiteral("大图标"))
    {
        return 128;
    }
    return 16;
}

std::filesystem::path to_fs_path(const QString &text)
{
#ifdef _WIN32
    return std::filesystem::path(text.toStdWString());
#else
    return std::filesystem::path(text.toStdString());
#endif
}

QString to_qstr(const std::filesystem::path &path)
{
#ifdef _WIN32
    return QString::fromStdWString(path.wstring());
#else
    return QString::fromStdString(path.string());
#endif
}

std::filesystem::path detect_repository()
{
    constexpr int max_depth = 5;
    auto dir = std::filesystem::current_path();
    for (int depth = 0; depth < max_depth; ++depth)
    {
        if (PolyFileManager::is_repository(dir))
        {
            return dir;
        }
        const auto parent = dir.parent_path();
        if (parent == dir)
        {
            break;
        }
        dir = parent;
    }
    return {};
}

std::string item_tag(const QListWidgetItem *item)
{
    return item ? item->data(Qt::UserRole).toString().toStdString() : std::string();
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    show_name_only_ = (config_.setting("file_name_only") == "是");
    buildUi();
    restoreWindowState();
    restoreDisplayState();
    applyThumbnailSettings();
    applyUndoSettings();

    const auto remembered = config_.lastRepository();
    if (!remembered.empty() && PolyFileManager::is_repository(remembered))
    {
        loadRepository(remembered);
    }
    else
    {
        const auto repo = detect_repository();
        if (!repo.empty())
        {
            loadRepository(repo);
        }
        else
        {
            statusBar()->showMessage("未打开仓库：请在菜单中打开或新建仓库");
        }
    }
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ----- splitter: tags | files -----
    splitter_ = new QSplitter(Qt::Horizontal, central);
    root->addWidget(splitter_);

    auto *left = new QWidget(splitter_);
    auto *lv = new QVBoxLayout(left);
    lv->setContentsMargins(12, 12, 12, 12);
    lv->setSpacing(8);

    auto *tag_header = new QLabel("标签", left);
    QFont tag_font = tag_header->font();
    tag_font.setBold(true);
    tag_font.setPointSize(tag_font.pointSize() + 1);
    tag_header->setFont(tag_font);
    lv->addWidget(tag_header);

    tag_search_ = new QLineEdit(left);
    tag_search_->setPlaceholderText("搜索标签...");
    lv->addWidget(tag_search_);

    mode_combo_ = new QComboBox(left);
    mode_combo_->addItem("交集");
    mode_combo_->addItem("并集");
    lv->addWidget(mode_combo_);

    tag_list_ = new QListWidget(left);
    tag_list_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tag_list_->setContextMenuPolicy(Qt::CustomContextMenu);
    tag_list_->setAlternatingRowColors(false);
    tag_list_->setItemDelegate(new TagDelegate(tag_list_));
    lv->addWidget(tag_list_, 1);

    auto *buttons = new QHBoxLayout;
    clear_btn_ = new QPushButton("清除选择", left);
    buttons->addWidget(clear_btn_);
    lv->addLayout(buttons);
    splitter_->addWidget(left);

    auto *right = new QWidget(splitter_);
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(12, 12, 12, 12);
    rv->setSpacing(8);

    auto *file_header = new QLabel("文件", right);
    QFont file_font = file_header->font();
    file_font.setBold(true);
    file_font.setPointSize(file_font.pointSize() + 1);
    file_header->setFont(file_font);
    rv->addWidget(file_header);

    file_search_ = new QLineEdit(right);
    file_search_->setPlaceholderText("搜索文件...");
    rv->addWidget(file_search_);

    file_list_ = new QListWidget(right);
    file_list_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    file_list_->setContextMenuPolicy(Qt::CustomContextMenu);
    file_list_->setObjectName(QStringLiteral("fileList"));
    file_list_->setAlternatingRowColors(false);
    file_list_->setUniformItemSizes(true);
    file_list_->setSpacing(3);
    file_list_->viewport()->installEventFilter(this);
    rv->addWidget(file_list_, 1);

    status_label_ = new QLabel(right);
    rv->addWidget(status_label_);
    splitter_->addWidget(right);

    splitter_->setStretchFactor(0, 1);
    splitter_->setStretchFactor(1, 2);
    splitter_->setSizes({320, 620});

    setCentralWidget(central);
    setAcceptDrops(true);
    central->setAcceptDrops(true);

    setWindowTitle("PolyFileManager");
    resize(1000, 620);

    repo_indicator_ = new QLabel(this);
    repo_indicator_->setText("未打开仓库");
    statusBar()->addWidget(repo_indicator_);

    // ----- menus -----
    auto *repo_menu = menuBar()->addMenu("仓库");
    repo_menu->addAction("打开仓库...", this, &MainWindow::openRepository);
    repo_menu->addAction("新建仓库...", this, &MainWindow::createRepository);
    repo_menu->addAction("管理...", this, &MainWindow::manageRepositories);
    repo_menu->addAction("删除仓库...", this, &MainWindow::destroyRepository);
    repo_menu->addSeparator();
    auto *quit_action = repo_menu->addAction("退出");
    connect(quit_action, &QAction::triggered, this, &QWidget::close);

    auto *edit_menu = menuBar()->addMenu("编辑");
    undo_action_ = edit_menu->addAction("撤销");
    undo_action_->setShortcut(QKeySequence::Undo);
    undo_action_->setEnabled(false);
    connect(undo_action_, &QAction::triggered, this, &MainWindow::undo);

    auto *tag_menu = menuBar()->addMenu("标签");
    tag_menu->addAction("添加标签...", this, &MainWindow::addTag);
    tag_menu->addAction("重命名标签...", this, &MainWindow::renameTag);
    tag_menu->addAction("删除标签...", this, &MainWindow::removeTag);

    auto *path_menu = menuBar()->addMenu("文件");
    path_menu->addAction("添加路径...", this, &MainWindow::addPath);
    path_menu->addAction("替换文件路径...", this, &MainWindow::replacePath);
    path_menu->addAction("删除路径...", this, &MainWindow::removePath);
    path_menu->addSeparator();
    path_menu->addAction("从文件夹导入路径...", this, &MainWindow::importFromFolder);
    path_menu->addSeparator();
    path_menu->addAction("编辑标签...", this, &MainWindow::editFileTags);

    auto *maintenance_menu = menuBar()->addMenu("维护");
    maintenance_menu->addAction("检查一致性", this, &MainWindow::checkConsistency);
    maintenance_menu->addAction("同步到仓库（数据→仓库）", this, &MainWindow::syncToRepo);
    maintenance_menu->addAction("从仓库同步（仓库→数据）", this, &MainWindow::syncFromRepo);

    auto *view_menu = menuBar()->addMenu("显示");
    auto *file_view_menu = view_menu->addMenu("文件显示");
    view_group_ = new QActionGroup(this);
    view_group_->setExclusive(true);
    const auto add_view_preset = [&](const QString &name, int size) {
        auto *action = file_view_menu->addAction(name);
        action->setCheckable(true);
        action->setData(size);
        view_group_->addAction(action);
    };
    add_view_preset(QStringLiteral("列表"), 16);
    add_view_preset(QStringLiteral("小图标"), 32);
    add_view_preset(QStringLiteral("中图标"), 64);
    add_view_preset(QStringLiteral("大图标"), 128);
    connect(view_group_, &QActionGroup::triggered, this,
            [this](QAction *action) { setViewSize(action->data().toInt()); });

    auto *settings_menu = menuBar()->addMenu("设置");
    settings_menu->addAction("设置...", this, &MainWindow::openSettings);

    // Ensure file drops reach the window instead of being caught by the lists.
    tag_list_->setDragDropMode(QAbstractItemView::NoDragDrop);
    file_list_->setDragDropMode(QAbstractItemView::NoDragDrop);
    tag_list_->viewport()->setAcceptDrops(false);
    file_list_->viewport()->setAcceptDrops(false);

    // ----- signals -----
    connect(tag_search_, &QLineEdit::textChanged, this, &MainWindow::onTagSearchChanged);
    connect(file_search_, &QLineEdit::textChanged, this, &MainWindow::onFileSearchChanged);
    connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onModeChanged);
    connect(tag_list_, &QListWidget::itemSelectionChanged, this, &MainWindow::onTagSelectionChanged);
    connect(tag_list_, &QListWidget::itemChanged, this, &MainWindow::onTagItemChanged);
    connect(tag_list_, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item && item->data(kAddItemRole).toInt() == 1)
        {
            tag_list_->editItem(item);
        }
    });
    connect(file_list_, &QListWidget::itemSelectionChanged, this, &MainWindow::onFileSelectionChanged);
    connect(file_list_, &QListWidget::itemDoubleClicked, this, &MainWindow::onFileDoubleClicked);
    connect(&thumbnailer_, &Thumbnailer::thumbnailReady, this, &MainWindow::onThumbnailReady);
    connect(clear_btn_, &QPushButton::clicked, this, &MainWindow::clearSelection);
    connect(tag_list_, &QListWidget::customContextMenuRequested, this, &MainWindow::onTagContextMenu);
    connect(file_list_, &QListWidget::customContextMenuRequested, this, &MainWindow::onFileContextMenu);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == file_list_->viewport() && event->type() == QEvent::Wheel)
    {
        auto *wheel = static_cast<QWheelEvent *>(event);
        if (wheel->modifiers() & Qt::ControlModifier)
        {
            const int dir = wheel->angleDelta().y() > 0 ? 1 : -1;
            cycleViewSize(dir);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setViewSize(int size)
{
    if (size < 16)
    {
        size = 16;
    }
    if (size > 256)
    {
        size = 256;
    }
    view_size_ = size;

    const bool icon_mode = size > 20;
    file_list_->setViewMode(icon_mode ? QListView::IconMode : QListView::ListMode);
    file_list_->setIconSize(QSize(size, size));
    file_list_->setMovement(QListView::Static);
    file_list_->setResizeMode(QListView::Adjust);
    file_list_->setWordWrap(icon_mode);
    file_list_->setSpacing(icon_mode ? 8 : 3);
    file_list_->setGridSize(icon_mode ? QSize(size + 24, size + 40) : QSize());
    syncViewMenu();
    refreshFileList();

    config_.setSetting("file_icon_size", std::to_string(size));
    config_.setSetting("file_view_mode", presetNameForSize(size).toStdString());
}

void MainWindow::cycleViewSize(int dir)
{
    int index = 0;
    int best = 1 << 30;
    for (int i = 0; i < static_cast<int>(kIconSizes.size()); ++i)
    {
        const int dist = std::abs(view_size_ - kIconSizes[i]);
        if (dist < best)
        {
            best = dist;
            index = i;
        }
    }
    int next = index + dir;
    if (next < 0)
    {
        next = 0;
    }
    if (next >= static_cast<int>(kIconSizes.size()))
    {
        next = static_cast<int>(kIconSizes.size()) - 1;
    }
    setViewSize(kIconSizes[next]);
}

void MainWindow::syncViewMenu()
{
    if (!view_group_)
    {
        return;
    }
    const int preset_size = presetSizeForName(presetNameForSize(view_size_));
    for (auto *action : view_group_->actions())
    {
        if (action->data().toInt() == preset_size)
        {
            action->setChecked(true);
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
    {
        return;
    }
    if (!requireManager("请先打开仓库，再拖入文件登记路径"))
    {
        return;
    }

    std::size_t added = 0;
    const auto before = manager_->current_data();
    for (const QUrl &url : event->mimeData()->urls())
    {
        const QString local = url.toLocalFile();
        if (local.isEmpty())
        {
            continue;
        }
        const auto path = to_fs_path(local);
        if (path.is_absolute() && manager_->add_path(path))
        {
            ++added;
        }
    }

    if (added > 0)
    {
        pushUndo(before);
        commit();
        statusBar()->showMessage(QString("已登记 %1 个路径").arg(added), 3000);
    }
    else
    {
        statusBar()->showMessage("没有新增路径", 3000);
    }
}

void MainWindow::restoreWindowState()
{
    const auto window_parts = QString::fromStdString(config_.setting("window_size")).split(QLatin1Char('x'));
    if (window_parts.size() == 2)
    {
        bool ok_w = false;
        bool ok_h = false;
        const int w = window_parts.at(0).toInt(&ok_w);
        const int h = window_parts.at(1).toInt(&ok_h);
        if (ok_w && ok_h && w >= 400 && h >= 300)
        {
            resize(w, h);
        }
    }

    if (splitter_)
    {
        QList<int> sizes;
        const auto pieces = QString::fromStdString(config_.setting("splitter_sizes")).split(QLatin1Char(','));
        for (const auto &piece : pieces)
        {
            bool ok = false;
            const int value = piece.trimmed().toInt(&ok);
            if (ok)
            {
                sizes.push_back(value);
            }
        }
        if (sizes.size() == splitter_->count())
        {
            splitter_->setSizes(sizes);
        }
    }
}

void MainWindow::restoreDisplayState()
{
    int size = 16;
    const std::string size_str = config_.setting("file_icon_size");
    if (!size_str.empty())
    {
        try
        {
            size = std::stoi(size_str);
        }
        catch (...)
        {
            size = 16;
        }
    }
    else
    {
        const std::string preset = config_.setting("file_view_mode");
        if (!preset.empty())
        {
            size = presetSizeForName(QString::fromStdString(preset));
        }
    }
    if (size < 16)
    {
        size = 16;
    }
    if (size > 256)
    {
        size = 256;
    }
    setViewSize(size);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const QSize sz = isMaximized() ? normalGeometry().size() : size();
    config_.setSetting("window_size", QString("%1x%2").arg(sz.width()).arg(sz.height()).toStdString());

    if (splitter_)
    {
        QStringList parts;
        for (int value : splitter_->sizes())
        {
            parts << QString::number(value);
        }
        config_.setSetting("splitter_sizes", parts.join(QLatin1Char(',')).toStdString());
    }
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Delete only removes the selected entries from the repository management
    // (registry + links/associations), never the real files on disk.
    if (event->key() == Qt::Key_Delete && file_list_ && file_list_->hasFocus() && !selectedPaths().empty())
    {
        removeSelectedPaths(false);
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

bool MainWindow::requireManager(const QString &message) const
{
    if (manager_)
    {
        return true;
    }
    QMessageBox::information(const_cast<MainWindow *>(this), "提示",
                             message.isEmpty() ? "请先打开或新建一个仓库" : message);
    return false;
}

void MainWindow::openRepository()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "选择仓库目录");
    if (!dir.isEmpty())
    {
        loadRepository(to_fs_path(dir));
    }
}

void MainWindow::createRepository()
{
    const QString dir = QFileDialog::getExistingDirectory(this, "选择新建仓库的目录");
    if (dir.isEmpty())
    {
        return;
    }
    const auto repo = to_fs_path(dir);
    if (PolyFileManager::is_repository(repo))
    {
        QMessageBox::information(this, "新建仓库", "该目录已经是仓库");
        return;
    }
    if (!PolyFileManager::create_repository(repo))
    {
        QMessageBox::warning(this, "新建仓库", "创建仓库失败");
        return;
    }
    loadRepository(repo);
}

void MainWindow::destroyRepository()
{
    if (!manager_)
    {
        QMessageBox::information(this, "删除仓库", "尚未打开仓库");
        return;
    }
    const auto repo = manager_->repo_path();
    if (QMessageBox::question(this, "删除仓库",
                              QString("确定删除仓库 %1？\n将移除数据文件与所有标签目录。").arg(to_qstr(repo))) !=
        QMessageBox::Yes)
    {
        return;
    }
    if (!PolyFileManager::destroy_repository(repo))
    {
        QMessageBox::warning(this, "删除仓库", "删除失败");
        return;
    }
    manager_.reset();
    active_tags_.clear();
    highlight_tags_.clear();
    checkbox_mode_ = false;
    tag_list_->clear();
    file_list_->clear();
    setWindowTitle("PolyFileManager");
    repo_indicator_->setText("未打开仓库");
    statusBar()->showMessage("仓库已删除");
    config_.clearLastRepository();
    config_.removeFromHistory(repo);
    refreshSelectionState();
}

void MainWindow::manageRepositories()
{
    RepoManagerDialog dialog(config_, this);
    connect(&dialog, &RepoManagerDialog::openRequested, this,
            [this](const QString &path) { loadRepository(to_fs_path(path)); });
    dialog.exec();
}

bool MainWindow::loadRepository(const std::filesystem::path &repo)
{
    if (!PolyFileManager::is_repository(repo))
    {
        QMessageBox::warning(this, "打开仓库", "该目录不是仓库");
        manager_.reset();
        return false;
    }
    try
    {
        manager_.emplace(repo);
    }
    catch (const std::exception &e)
    {
        QMessageBox::warning(this, "打开仓库", QString::fromStdString(e.what()));
        manager_.reset();
        return false;
    }

    setWindowTitle(QString("PolyFileManager - %1").arg(to_qstr(repo)));
    repo_indicator_->setText(QStringLiteral("仓库：%1").arg(to_qstr(repo)));
    active_tags_.clear();
    highlight_tags_.clear();
    checkbox_mode_ = false;
    tag_search_->clear();
    file_search_->clear();
    config_.setLastRepository(repo);
    config_.addToHistory(repo);
    refreshTagList();
    refreshFileList();
    refreshSelectionState();
    return true;
}

void MainWindow::refreshTagList()
{
    if (!manager_)
    {
        tag_list_->clear();
        return;
    }

    std::set<FileTag> top;
    std::set<FileTag> rest;
    for (const auto &tag : manager_->tags())
    {
        (highlight_tags_.count(tag) ? top : rest).insert(tag);
    }

    const QString filter = tag_search_->text().trimmed();
    tag_list_->blockSignals(true);
    tag_list_->clear();

    const auto add_tag = [&](const FileTag &tag) {
        const QString name = QString::fromStdString(tag);
        const QString display =
            name + QStringLiteral("  (") + QString::number(manager_->paths_of_tag(tag).size()) + QStringLiteral(")");
        auto *item = new QListWidgetItem(display, tag_list_);
        item->setData(Qt::UserRole, name);

        if (checkbox_mode_)
        {
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(active_tags_.count(tag) ? Qt::Checked : Qt::Unchecked);
        }
        if (active_tags_.count(tag))
        {
            item->setSelected(true);
        }
        if (highlight_tags_.count(tag))
        {
            item->setData(kTagHighlightRole, 1);
        }
        if (!filter.isEmpty() && !name.contains(filter, Qt::CaseInsensitive))
        {
            item->setHidden(true);
        }
    };

    for (const auto &tag : top)
    {
        add_tag(tag);
    }
    for (const auto &tag : rest)
    {
        add_tag(tag);
    }

    // Trailing "add a tag" placeholder (click to edit, Enter to commit).
    auto *add_item = new QListWidgetItem(QStringLiteral("+ 添加标签"), tag_list_);
    add_item->setData(Qt::UserRole, QString());
    add_item->setData(kAddItemRole, 1);
    add_item->setFlags(add_item->flags() | Qt::ItemIsEditable);
    QFont add_font = add_item->font();
    add_font.setItalic(true);
    add_item->setFont(add_font);
    add_item->setForeground(QBrush(QColor(160, 160, 160)));

    tag_list_->blockSignals(false);
}

void MainWindow::refreshFileList()
{
    if (!manager_)
    {
        file_list_->clear();
        return;
    }

    std::vector<FilePath> files;
    if (!active_tags_.empty())
    {
        const std::vector<FileTag> tags(active_tags_.begin(), active_tags_.end());
        const auto matched = paths_for_tags(tags);
        files.assign(matched.begin(), matched.end());
    }
    else
    {
        const auto all = manager_->paths();
        files.assign(all.begin(), all.end());
    }

    const QString filter = file_search_->text().trimmed();
    file_list_->blockSignals(true);
    file_list_->clear();
    for (const auto &path : files)
    {
        const QString full = to_qstr(path);
        if (!filter.isEmpty() && !full.contains(filter, Qt::CaseInsensitive))
        {
            continue;
        }
        const bool icon_mode = view_size_ > 20;
        const QString display =
            (icon_mode || show_name_only_) ? QString::fromStdString(path.filename().string()) : full;
        auto *item = new QListWidgetItem(display, file_list_);
        item->setData(Qt::UserRole, full);
        static QFileIconProvider icon_provider;
        const QFileInfo file_info(to_qstr(path));
        QIcon icon = icon_provider.icon(file_info);
        if (thumbnailer_.canThumbnail(to_qstr(path), view_size_))
        {
            const QIcon thumb = thumbnailer_.cached(to_qstr(path), view_size_);
            if (!thumb.isNull())
            {
                icon = thumb;
            }
            else
            {
                thumbnailer_.request(to_qstr(path), view_size_);
            }
        }
        item->setIcon(icon);
        if (!icon_mode)
        {
            item->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        }
        QStringList tag_names;
        for (const auto &tag : manager_->tags_of_file(path))
        {
            tag_names << QString::fromStdString(tag);
        }
        QString tip = full;
        if (!tag_names.isEmpty())
        {
            tip += QStringLiteral("\n标签：") + tag_names.join(QStringLiteral(", "));
        }
        item->setToolTip(tip);
    }
    file_list_->blockSignals(false);
    refreshSelectionState();
}

void MainWindow::refreshSelectionState()
{
    if (status_label_ && manager_)
    {
        status_label_->setText(QString("标签 %1  · 文件 %2").arg(manager_->tags().size()).arg(manager_->paths().size()));
    }
}

void MainWindow::onTagSelectionChanged()
{
    if (checkbox_mode_)
    {
        return;
    }
    active_tags_.clear();
    for (auto *item : tag_list_->selectedItems())
    {
        const std::string tag = item_tag(item);
        if (!tag.empty())
        {
            active_tags_.insert(tag);
        }
    }
    refreshFileList();
}

void MainWindow::onTagItemChanged(QListWidgetItem *item)
{
    if (item && item->data(kAddItemRole).toInt() == 1)
    {
        addTagFromPlaceholder(item);
        return;
    }
    if (!checkbox_mode_)
    {
        return;
    }
    active_tags_.clear();
    for (int i = 0; i < tag_list_->count(); ++i)
    {
        auto *item = tag_list_->item(i);
        if (item->data(kAddItemRole).toInt() == 1)
        {
            continue;
        }
        if (item->checkState() == Qt::Checked)
        {
            active_tags_.insert(item_tag(item));
        }
    }
    refreshFileList();
}

void MainWindow::onFileSelectionChanged()
{
    highlight_tags_ = tags_for_paths(selectedPaths());
    refreshTagList();
}

std::set<FilePath> MainWindow::paths_for_tags(const std::vector<FileTag> &tags) const
{
    if (!manager_ || tags.empty())
    {
        return {};
    }
    if (mode_combo_->currentIndex() == 1)
    {
        std::set<FilePath> result;
        for (const auto &tag : tags)
        {
            const auto paths = manager_->paths_of_tag(tag);
            result.insert(paths.begin(), paths.end());
        }
        return result;
    }
    return manager_->paths_with_tags(tags);
}

std::set<FileTag> MainWindow::tags_for_paths(const std::vector<FilePath> &paths) const
{
    if (!manager_ || paths.empty())
    {
        return {};
    }
    if (mode_combo_->currentIndex() == 1)
    {
        std::set<FileTag> result;
        for (const auto &path : paths)
        {
            const auto tags = manager_->tags_of_file(path);
            result.insert(tags.begin(), tags.end());
        }
        return result;
    }
    return manager_->tags_with_paths(paths);
}

void MainWindow::enterCheckboxMode()
{
    if (!manager_)
    {
        return;
    }
    checkbox_mode_ = true;
    const auto *current = tag_list_->currentItem();
    if (current)
    {
        active_tags_.insert(item_tag(current));
    }
    refreshTagList();
    refreshFileList();
    statusBar()->showMessage("已进入多选模式：点击标签可加入/移出选择，右键可退出多选", 4000);
}

void MainWindow::finishCheckboxMode()
{
    checkbox_mode_ = false;
    // Rebuild the list so the checkboxes disappear and the active tags are
    // represented as ordinary selection instead.
    refreshTagList();
    refreshFileList();
}

void MainWindow::clearSelection()
{
    if (checkbox_mode_)
    {
        finishCheckboxMode();
    }
    active_tags_.clear();
    highlight_tags_.clear();
    tag_list_->clearSelection();
    tag_list_->clear();
    refreshTagList();
    refreshFileList();
}

void MainWindow::onTagSearchChanged(const QString &)
{
    refreshTagList();
}

void MainWindow::onFileSearchChanged(const QString &)
{
    refreshFileList();
}

void MainWindow::onModeChanged(int)
{
    refreshFileList();
    highlight_tags_ = tags_for_paths(selectedPaths());
    refreshTagList();
}

void MainWindow::addTagFromPlaceholder(QListWidgetItem *item)
{
    const QString placeholder = QStringLiteral("+ 添加标签");
    const QString text = item->text().trimmed();

    const auto reset = [&] {
        tag_list_->blockSignals(true);
        item->setText(placeholder);
        tag_list_->blockSignals(false);
    };

    if (text.isEmpty() || text == placeholder || text == QStringLiteral("+"))
    {
        reset();
        return;
    }

    const std::string tag = text.toStdString();
    const auto before = manager_->current_data();
    if (manager_->add_tag(tag))
    {
        pushUndo(before);
        commit(); // rebuilds the list, which re-appends the placeholder
        statusBar()->showMessage("已添加标签", 3000);
    }
    else
    {
        reset();
        statusBar()->showMessage("标签已存在或创建失败", 3000);
    }
}

// ----- CRUD helpers -----

std::string MainWindow::selectedTag() const
{
    auto *item = tag_list_->currentItem();
    return item_tag(item);
}

std::vector<std::filesystem::path> MainWindow::selectedPaths() const
{
    std::vector<std::filesystem::path> result;
    for (auto *item : file_list_->selectedItems())
    {
        result.push_back(to_fs_path(item->data(Qt::UserRole).toString()));
    }
    return result;
}

bool MainWindow::commit()
{
    if (!manager_)
    {
        return false;
    }
    if (!manager_->save())
    {
        QMessageBox::warning(this, "保存失败", "无法保存数据文件");
        return false;
    }
    refreshTagList();
    refreshFileList();
    refreshSelectionState();
    statusBar()->showMessage("已保存", 1500);
    return true;
}

void MainWindow::addTag()
{
    if (!requireManager(""))
    {
        return;
    }
    bool ok = false;
    const QString name =
        QInputDialog::getText(this, "添加标签", "标签名称：", QLineEdit::Normal, QString(), &ok).trimmed();
    if (!ok || name.isEmpty())
    {
        return;
    }
    const auto before = manager_->current_data();
    if (!manager_->add_tag(name.toStdString()))
    {
        QMessageBox::information(this, "添加标签", "标签已存在或创建失败");
        return;
    }
    pushUndo(before);
    commit();
}

void MainWindow::renameTag()
{
    if (!requireManager(""))
    {
        return;
    }
    const std::string old_tag = selectedTag();
    if (old_tag.empty())
    {
        QMessageBox::information(this, "重命名标签", "请先在左侧选择一个标签");
        return;
    }
    bool ok = false;
    const QString name = QInputDialog::getText(this, "重命名标签", "新名称：", QLineEdit::Normal,
                                               QString::fromStdString(old_tag), &ok)
                             .trimmed();
    if (!ok || name.isEmpty())
    {
        return;
    }
    const auto before = manager_->current_data();
    if (!manager_->rename_tag(old_tag, name.toStdString()))
    {
        QMessageBox::warning(this, "重命名标签", "重命名失败（新标签可能已存在）");
        return;
    }
    pushUndo(before);
    commit();
}

void MainWindow::removeTag()
{
    if (!requireManager(""))
    {
        return;
    }
    const std::string tag = selectedTag();
    if (tag.empty())
    {
        QMessageBox::information(this, "删除标签", "请先在左侧选择一个标签");
        return;
    }
    if (QMessageBox::question(this, "删除标签", QString("删除标签“%1”及其所有关联？").arg(QString::fromStdString(tag))) !=
        QMessageBox::Yes)
    {
        return;
    }
    const auto before = manager_->current_data();
    if (!manager_->remove_tag(tag))
    {
        QMessageBox::warning(this, "删除标签", "删除失败");
        return;
    }
    pushUndo(before);
    commit();
}

void MainWindow::addPath()
{
    if (!requireManager(""))
    {
        return;
    }
    bool ok = false;
    const QString path_text =
        QInputDialog::getText(this, "添加路径", "文件或目录的绝对路径：", QLineEdit::Normal, QString(), &ok).trimmed();
    const auto path = to_fs_path(path_text);
    if (!ok || !path.is_absolute())
    {
        QMessageBox::information(this, "添加路径", "请输入合法的绝对路径");
        return;
    }
    const auto before = manager_->current_data();
    if (!manager_->add_path(path))
    {
        QMessageBox::information(this, "添加路径", "路径无效或已存在");
        return;
    }
    pushUndo(before);
    commit();
}

void MainWindow::replacePath()
{
    if (!requireManager(""))
    {
        return;
    }
    const auto paths = selectedPaths();
    if (paths.size() != 1)
    {
        QMessageBox::information(this, "替换文件路径", "请先在右侧选择一个文件");
        return;
    }
    bool ok = false;
    const QString new_text = QInputDialog::getText(this, "替换文件路径", "新路径：", QLineEdit::Normal,
                                                   to_qstr(paths.front()), &ok)
                                 .trimmed();
    const auto new_path = to_fs_path(new_text);
    if (!ok || new_path.empty())
    {
        return;
    }
    if (!std::filesystem::exists(new_path))
    {
        QMessageBox::information(this, "替换文件路径", "新路径不存在");
        return;
    }
    const auto before = manager_->current_data();
    if (!manager_->replace_path(paths.front(), new_path))
    {
        QMessageBox::warning(this, "替换文件路径", "替换失败");
        return;
    }
    pushUndo(before);
    commit();
}

void MainWindow::removePath()
{
    removeSelectedPaths(true);
}

void MainWindow::removeSelectedPaths(bool confirm)
{
    if (!requireManager(""))
    {
        return;
    }
    const auto paths = selectedPaths();
    if (paths.empty())
    {
        QMessageBox::information(this, "删除路径", "请先在右侧选择文件");
        return;
    }
    if (confirm &&
        QMessageBox::question(this, "删除路径", QString("删除 %1 个文件条目？").arg(paths.size())) != QMessageBox::Yes)
    {
        return;
    }
    const auto before = manager_->current_data();
    for (const auto &path : paths)
    {
        if (!manager_->remove_path(path))
        {
            QMessageBox::warning(this, "删除路径", QString("删除失败：%1").arg(to_qstr(path)));
            return;
        }
    }
    pushUndo(before);
    commit();
}

void MainWindow::importFromFolder()
{
    if (!requireManager(""))
    {
        return;
    }
    const QString dir = QFileDialog::getExistingDirectory(this, "选择要导入的文件夹");
    if (dir.isEmpty())
    {
        return;
    }

    ImportDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    const auto folder = to_fs_path(dir);
    const auto paths = collectFolderPaths(folder, dialog.includeFolder(), dialog.recursive());
    const std::string tag = dialog.tagName().toStdString();
    const auto before = manager_->current_data();

    if (!tag.empty())
    {
        manager_->add_tag(tag);
    }

    std::size_t added = 0;
    std::size_t tagged = 0;
    for (const auto &path : paths)
    {
        if (manager_->add_path(path))
        {
            ++added;
        }
        if (!tag.empty() && manager_->assign_tag(path, tag))
        {
            ++tagged;
        }
    }

    if (added == 0 && tagged == 0)
    {
        statusBar()->showMessage("没有新增内容", 3000);
        return;
    }

    pushUndo(before);
    commit();
    statusBar()->showMessage(QString("导入完成：路径 %1 个，其中打标签 %2 个").arg(added).arg(tagged), 4000);
}

std::vector<std::filesystem::path> MainWindow::collectFolderPaths(const std::filesystem::path &folder,
                                                                  bool includeFolder, bool recursive) const
{
    std::vector<std::filesystem::path> result;
    std::error_code ec;

    if (includeFolder)
    {
        result.push_back(folder);
    }

    if (recursive)
    {
        for (auto it = std::filesystem::recursive_directory_iterator(folder, ec);
             it != std::filesystem::recursive_directory_iterator(); ++it)
        {
            if (ec)
            {
                break;
            }
            if (it->is_regular_file(ec))
            {
                result.push_back(it->path());
            }
        }
    }
    else
    {
        for (auto it = std::filesystem::directory_iterator(folder, ec);
             it != std::filesystem::directory_iterator(); ++it)
        {
            if (ec)
            {
                break;
            }
            if (it->is_regular_file(ec))
            {
                result.push_back(it->path());
            }
        }
    }
    return result;
}

// ----- associations -----

void MainWindow::editFileTags()
{
    if (!requireManager(""))
    {
        return;
    }
    const auto paths = selectedPaths();
    if (paths.empty())
    {
        QMessageBox::information(this, "编辑标签", "请先在右侧选择文件");
        return;
    }

    QStringList tags;
    for (const auto &tag : manager_->tags())
    {
        tags << QString::fromStdString(tag);
    }
    QStringList files;
    for (const auto &path : paths)
    {
        files << to_qstr(path);
    }
    const auto is_assigned = [this](const QString &tag, const QString &file) {
        return manager_->tags_of_file(to_fs_path(file)).count(tag.toStdString()) > 0;
    };

    EditTagsDialog dialog(tags, files, is_assigned, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }
    const auto checked = dialog.checkedPairs();
    const QString new_tag = dialog.newTag();

    const auto before = manager_->current_data();
    bool changed = false;

    // Build tag -> set of files that should end up assigned.
    QHash<QString, QSet<QString>> assign_map;
    for (const auto &pair : checked)
    {
        assign_map[pair.first].insert(pair.second);
    }

    // Per-file diff against the current state.
    for (const auto &tag : manager_->tags())
    {
        const QString tag_q = QString::fromStdString(tag);
        for (const auto &path : paths)
        {
            const QString file_q = to_qstr(path);
            const bool should_assign = assign_map.value(tag_q).contains(file_q);
            const bool current = manager_->tags_of_file(path).count(tag) > 0;
            if (should_assign && !current)
            {
                manager_->add_tag(tag);
                manager_->add_path(path);
                if (!manager_->assign_tag(path, tag))
                {
                    QMessageBox::warning(this, "编辑标签", QString("指派失败：%1").arg(to_qstr(path)));
                    return;
                }
                changed = true;
            }
            else if (!should_assign && current)
            {
                if (!manager_->unassign_tag(path, tag))
                {
                    QMessageBox::warning(this, "编辑标签", QString("移除失败：%1").arg(to_qstr(path)));
                    return;
                }
                changed = true;
            }
        }
    }

    // New tag -> assign to every selected file.
    if (!new_tag.isEmpty())
    {
        manager_->add_tag(new_tag.toStdString());
        for (const auto &path : paths)
        {
            manager_->add_path(path);
            if (!manager_->assign_tag(path, new_tag.toStdString()))
            {
                QMessageBox::warning(this, "编辑标签", QString("指派失败：%1").arg(to_qstr(path)));
                return;
            }
        }
        changed = true;
    }

    if (!changed)
    {
        statusBar()->showMessage("没有变化", 2500);
        return;
    }
    pushUndo(before);
    commit();
    statusBar()->showMessage("已更新标签指派", 2500);
}

// ----- maintenance -----

void MainWindow::checkConsistency()
{
    if (!requireManager(""))
    {
        return;
    }
    const bool ok = manager_->is_consistent();
    const QString msg = ok ? "数据与仓库一致" : "数据与仓库不一致";
    statusBar()->showMessage(msg, 3000);
    QMessageBox::information(this, "检查一致性", msg);
}

void MainWindow::syncToRepo()
{
    if (!requireManager(""))
    {
        return;
    }
    if (!manager_->sync())
    {
        QMessageBox::warning(this, "同步", "同步到仓库失败");
        return;
    }
    refreshSelectionState();
    statusBar()->showMessage("已按数据重建仓库", 3000);
}

void MainWindow::syncFromRepo()
{
    if (!requireManager(""))
    {
        return;
    }
    if (!manager_->rebuild_from_repository() || !manager_->save())
    {
        QMessageBox::warning(this, "同步", "从仓库同步失败");
        return;
    }
    commit();
    statusBar()->showMessage("已按仓库重建数据", 3000);
}

void MainWindow::openSettings()
{
    static const std::vector<SettingDef> defs = {
        {QStringLiteral("file_name_only"), QStringLiteral("仅显示文件名"),
         {QStringLiteral("否"), QStringLiteral("是")}},
        {QStringLiteral("file_view_mode"), QStringLiteral("文件显示方式"),
         {QStringLiteral("列表"), QStringLiteral("小图标"), QStringLiteral("中图标"), QStringLiteral("大图标")}},
        {QStringLiteral("thumb_min_size"), QStringLiteral("生成缩略图的最小尺寸"),
         {QStringLiteral("关闭"), QStringLiteral("32"), QStringLiteral("48"), QStringLiteral("64"),
          QStringLiteral("96"), QStringLiteral("128")}},
        {QStringLiteral("thumb_cache_mb"), QStringLiteral("缩略图缓存大小(MB)"), {}, QStringLiteral("16"), true},
        {QStringLiteral("undo_max"), QStringLiteral("可撤销的操作数"), {}, QStringLiteral("50"), true},
    };

    settings_view_before_ = config_.setting("file_view_mode");
    SettingsDialog dialog(config_, defs, this);
    connect(&dialog, &SettingsDialog::settingsSaved, this, &MainWindow::applySettings);
    dialog.exec();
}

void MainWindow::applySettings()
{
    show_name_only_ = (config_.setting("file_name_only") == "是");
    applyThumbnailSettings();
    applyUndoSettings();
    const std::string new_view = config_.setting("file_view_mode");
    if (!new_view.empty() && new_view != settings_view_before_)
    {
        setViewSize(presetSizeForName(QString::fromStdString(new_view)));
    }
    refreshFileList();
    statusBar()->showMessage("设置已应用", 2500);
}

void MainWindow::applyThumbnailSettings()
{
    std::string min_str = config_.setting("thumb_min_size");
    if (min_str.empty())
    {
        min_str = "48";
        config_.setSetting("thumb_min_size", min_str);
    }
    int min = 48;
    if (min_str == "关闭")
    {
        min = 100000;
    }
    else
    {
        try
        {
            min = std::stoi(min_str);
        }
        catch (...)
        {
            min = 48;
        }
    }
    thumbnailer_.setMinSize(min);

    std::string mb_str = config_.setting("thumb_cache_mb");
    if (mb_str.empty())
    {
        mb_str = "16";
        config_.setSetting("thumb_cache_mb", mb_str);
    }
    int mb = 16;
    try
    {
        mb = std::stoi(mb_str);
    }
    catch (...)
    {
        mb = 16;
    }
    if (mb < 1)
    {
        mb = 1;
    }
    thumbnailer_.setCacheMb(mb);
}

void MainWindow::applyUndoSettings()
{
    std::string value = config_.setting("undo_max");
    if (value.empty())
    {
        value = "50";
        config_.setSetting("undo_max", value);
    }
    try
    {
        const int parsed = std::stoi(value);
        max_undo_ = parsed < 0 ? 0 : parsed;
    }
    catch (...)
    {
        max_undo_ = 50;
    }
    while (static_cast<int>(undo_stack_.size()) > max_undo_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
    updateUndoAction();
}

void MainWindow::pushUndo(const FileTagData &snapshot)
{
    undo_stack_.push_back(snapshot);
    while (static_cast<int>(undo_stack_.size()) > max_undo_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
    updateUndoAction();
}

void MainWindow::updateUndoAction()
{
    if (undo_action_)
    {
        undo_action_->setEnabled(!undo_stack_.empty());
    }
}

void MainWindow::undo()
{
    if (undo_stack_.empty() || !manager_)
    {
        statusBar()->showMessage("没有可撤销的操作", 2500);
        return;
    }

    const FileTagData snapshot = undo_stack_.back();
    undo_stack_.pop_back();
    if (!manager_->restore_data(snapshot))
    {
        QMessageBox::warning(this, "撤销", "撤销失败");
        return;
    }
    if (!manager_->save())
    {
        QMessageBox::warning(this, "撤销", "保存失败");
        return;
    }

    active_tags_.clear();
    highlight_tags_.clear();
    checkbox_mode_ = false;
    refreshTagList();
    refreshFileList();
    statusBar()->showMessage("已撤销", 2500);
    updateUndoAction();
}

// ----- context menus -----

void MainWindow::onTagContextMenu(const QPoint &pos)
{
    if (!manager_)
    {
        return;
    }
    if (auto *item = tag_list_->itemAt(pos))
    {
        tag_list_->setCurrentItem(item);
    }
    QMenu menu(this);
    if (checkbox_mode_)
    {
        menu.addAction("退出多选", this, &MainWindow::finishCheckboxMode);
    }
    else
    {
        menu.addAction("多选", this, &MainWindow::enterCheckboxMode);
    }
    menu.addSeparator();
    menu.addAction("重命名标签", this, &MainWindow::renameTag);
    menu.addAction("删除标签", this, &MainWindow::removeTag);
    menu.exec(tag_list_->viewport()->mapToGlobal(pos));
}

void MainWindow::onFileContextMenu(const QPoint &pos)
{
    if (!manager_)
    {
        return;
    }
    QMenu menu(this);
    menu.addAction("打开", this, [this] {
        const auto paths = selectedPaths();
        if (!paths.empty())
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(to_qstr(paths.front())));
        }
    });
    menu.addAction("在文件夹中显示", this, &MainWindow::revealSelectedPath);
    menu.addAction("复制路径", this, &MainWindow::copySelectedPath);
    menu.addSeparator();
    menu.addAction("编辑标签", this, &MainWindow::editFileTags);
    menu.addSeparator();
    menu.addAction("替换文件路径", this, &MainWindow::replacePath);
    menu.addAction("删除路径", this, &MainWindow::removePath);
    menu.exec(file_list_->viewport()->mapToGlobal(pos));
}

void MainWindow::onFileDoubleClicked(QListWidgetItem *item)
{
    if (!item)
    {
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(item->data(Qt::UserRole).toString()));
}

void MainWindow::onThumbnailReady(const QString &path, int size, const QImage &image)
{
    const QIcon icon(QPixmap::fromImage(image));
    thumbnailer_.store(path, size, icon);

    if (size != view_size_)
    {
        return;
    }
    for (int i = 0; i < file_list_->count(); ++i)
    {
        auto *item = file_list_->item(i);
        if (item && item->data(Qt::UserRole).toString() == path)
        {
            item->setIcon(icon);
            break;
        }
    }
}

void MainWindow::revealSelectedPath()
{
    const auto paths = selectedPaths();
    if (paths.empty())
    {
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(to_qstr(paths.front().parent_path())));
}

void MainWindow::copySelectedPath()
{
    const auto paths = selectedPaths();
    if (paths.empty())
    {
        return;
    }
    QGuiApplication::clipboard()->setText(to_qstr(paths.front()));
}
