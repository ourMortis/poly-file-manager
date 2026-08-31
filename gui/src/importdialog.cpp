#include "importdialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

ImportDialog::ImportDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("导入路径");

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("导入方式：", this));

    include_folder_ = new QCheckBox("同时包含所选文件夹本身", this);
    recursive_ = new QCheckBox("搜索子文件夹（递归）", this);
    layout->addWidget(include_folder_);
    layout->addWidget(recursive_);

    layout->addWidget(new QLabel("为导入的路径添加标签（留空则不加）：", this));
    tag_edit_ = new QLineEdit(this);
    tag_edit_->setPlaceholderText("例如：照片");
    layout->addWidget(tag_edit_);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    resize(360, 220);
}

bool ImportDialog::includeFolder() const
{
    return include_folder_->isChecked();
}

bool ImportDialog::recursive() const
{
    return recursive_->isChecked();
}

QString ImportDialog::tagName() const
{
    return tag_edit_->text().trimmed();
}
