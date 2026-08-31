#include "edittagsdialog.hpp"

#include <QDialogButtonBox>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

EditTagsDialog::EditTagsDialog(const QStringList &tags, const QStringList &files,
                               const std::function<bool(const QString &, const QString &)> &isAssigned,
                               QWidget *parent)
    : QDialog(parent), files_(files)
{
    setWindowTitle("编辑标签");
    resize(440, 480);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("点击标签复选框可批量指派/移除；点击左侧箭头展开，可逐个文件调整：", this));

    list_ = new QTreeWidget(this);
    list_->setHeaderHidden(true);
    list_->setColumnCount(1);
    list_->setRootIsDecorated(true);
    list_->setUniformRowHeights(true);
    list_->setAlternatingRowColors(false);
    list_->setIndentation(18);
    list_->setTextElideMode(Qt::ElideMiddle);

    for (const QString &tag : tags)
    {
        auto *tag_item = new QTreeWidgetItem(list_);
        tag_item->setText(0, tag);
        tag_item->setData(0, Qt::UserRole, tag);
        tag_item->setFlags(tag_item->flags() | Qt::ItemIsUserCheckable);

        int checked_count = 0;
        for (const QString &file : files)
        {
            auto *file_item = new QTreeWidgetItem(tag_item);
            file_item->setText(0, QFileInfo(file).fileName());
            file_item->setData(0, Qt::UserRole, file);
            file_item->setFlags(file_item->flags() | Qt::ItemIsUserCheckable);
            const bool has = isAssigned(tag, file);
            file_item->setCheckState(0, has ? Qt::Checked : Qt::Unchecked);
            if (has)
            {
                ++checked_count;
            }
        }
        if (checked_count == files.size())
        {
            tag_item->setCheckState(0, Qt::Checked);
        }
        else if (checked_count > 0)
        {
            tag_item->setCheckState(0, Qt::PartiallyChecked);
        }
        else
        {
            tag_item->setCheckState(0, Qt::Unchecked);
        }
        tag_item->setExpanded(false);
    }

    layout->addWidget(list_, 1);

    new_tag_ = new QLineEdit(this);
    new_tag_->setPlaceholderText("输入要新增并指派给所有选中文件的标签（可留空）");
    layout->addWidget(new_tag_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    connect(list_, &QTreeWidget::itemChanged, this, &EditTagsDialog::onItemChanged);
}

void EditTagsDialog::onItemChanged(QTreeWidgetItem *item, int column)
{
    if (column != 0)
    {
        return;
    }
    if (item->parent() == nullptr)
    {
        // Parent (tag) checkbox: apply to every file.
        const bool check = item->checkState(0) != Qt::Unchecked;
        list_->blockSignals(true);
        for (int i = 0; i < item->childCount(); ++i)
        {
            item->child(i)->setCheckState(0, check ? Qt::Checked : Qt::Unchecked);
        }
        list_->blockSignals(false);
    }
    else
    {
        // Child (file) checkbox: recompute the parent tri-state.
        auto *parent = item->parent();
        int checked = 0;
        const int total = parent->childCount();
        for (int i = 0; i < total; ++i)
        {
            if (parent->child(i)->checkState(0) == Qt::Checked)
            {
                ++checked;
            }
        }
        const Qt::CheckState parent_state =
            checked == 0 ? Qt::Unchecked : (checked == total ? Qt::Checked : Qt::PartiallyChecked);
        list_->blockSignals(true);
        parent->setCheckState(0, parent_state);
        list_->blockSignals(false);
    }
}

QVector<QPair<QString, QString>> EditTagsDialog::checkedPairs() const
{
    QVector<QPair<QString, QString>> result;
    for (int i = 0; i < list_->topLevelItemCount(); ++i)
    {
        auto *tag_item = list_->topLevelItem(i);
        const QString tag = tag_item->data(0, Qt::UserRole).toString();
        for (int j = 0; j < tag_item->childCount(); ++j)
        {
            auto *file_item = tag_item->child(j);
            if (file_item->checkState(0) == Qt::Checked)
            {
                result.append(qMakePair(tag, file_item->data(0, Qt::UserRole).toString()));
            }
        }
    }
    return result;
}

QString EditTagsDialog::newTag() const
{
    return new_tag_->text().trimmed();
}
