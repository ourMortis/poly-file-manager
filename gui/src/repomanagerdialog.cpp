#include "repomanagerdialog.hpp"

#include "poly_file_manager.hpp"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
std::filesystem::path to_fs_path(const QString &text)
{
#ifdef _WIN32
    return std::filesystem::path(text.toStdWString());
#else
    return std::filesystem::path(text.toStdString());
#endif
}
} // namespace

RepoManagerDialog::RepoManagerDialog(AppConfig &config, QWidget *parent) : QDialog(parent), config_(config)
{
    setWindowTitle("仓库管理");
    resize(560, 360);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("已打开过的仓库：", this));

    list_ = new QListWidget(this);
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(list_, 1);

    auto *buttons = new QHBoxLayout;
    auto *open_btn = new QPushButton("打开", this);
    auto *create_btn = new QPushButton("新建仓库", this);
    auto *remove_btn = new QPushButton("从列表移除", this);
    auto *destroy_btn = new QPushButton("销毁仓库", this);
    auto *refresh_btn = new QPushButton("刷新", this);
    auto *close_btn = new QPushButton("关闭", this);

    buttons->addWidget(open_btn);
    buttons->addWidget(create_btn);
    buttons->addWidget(remove_btn);
    buttons->addWidget(destroy_btn);
    buttons->addWidget(refresh_btn);
    buttons->addWidget(close_btn);
    layout->addLayout(buttons);

    connect(open_btn, &QPushButton::clicked, this, &RepoManagerDialog::onOpen);
    connect(create_btn, &QPushButton::clicked, this, &RepoManagerDialog::onCreate);
    connect(remove_btn, &QPushButton::clicked, this, &RepoManagerDialog::onRemove);
    connect(destroy_btn, &QPushButton::clicked, this, &RepoManagerDialog::onDestroy);
    connect(refresh_btn, &QPushButton::clicked, this, &RepoManagerDialog::refresh);
    connect(close_btn, &QPushButton::clicked, this, &QDialog::accept);

    rebuildList();
}

void RepoManagerDialog::rebuildList()
{
    list_->clear();
    for (const auto &repo : config_.history())
    {
        const QString text = QString::fromStdString(repo.string());
        const bool exists = PolyFileManager::is_repository(repo);
        auto *item = new QListWidgetItem(text + (exists ? QStringLiteral("    [存在]") : QStringLiteral("    [丢失]")),
                                         list_);
        item->setData(Qt::UserRole, text);
    }
}

void RepoManagerDialog::onOpen()
{
    auto *item = list_->currentItem();
    if (!item)
    {
        QMessageBox::information(this, "打开仓库", "请先选择一个仓库");
        return;
    }
    const QString path = item->data(Qt::UserRole).toString();
    config_.setLastRepository(to_fs_path(path));
    emit openRequested(path);
    accept();
}

void RepoManagerDialog::onCreate()
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
    config_.addToHistory(repo);
    rebuildList();
}

void RepoManagerDialog::onRemove()
{
    auto *item = list_->currentItem();
    if (!item)
    {
        QMessageBox::information(this, "从列表移除", "请先选择一个仓库");
        return;
    }
    config_.removeFromHistory(to_fs_path(item->data(Qt::UserRole).toString()));
    rebuildList();
}

void RepoManagerDialog::onDestroy()
{
    auto *item = list_->currentItem();
    if (!item)
    {
        QMessageBox::information(this, "销毁仓库", "请先选择一个仓库");
        return;
    }
    const auto repo = to_fs_path(item->data(Qt::UserRole).toString());
    if (QMessageBox::question(this, "销毁仓库",
                              QString("确定销毁仓库 %1？\n将删除其数据文件与所有标签目录。")
                                  .arg(QString::fromStdString(repo.string()))) != QMessageBox::Yes)
    {
        return;
    }
    if (PolyFileManager::destroy_repository(repo))
    {
        config_.removeFromHistory(repo);
    }
    rebuildList();
}

void RepoManagerDialog::refresh()
{
    rebuildList();
}
