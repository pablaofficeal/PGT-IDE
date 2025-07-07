#include "aftocomplet.h"
#include <QStringList>
#include <QCompleter>
#include <QTextEdit>

aftocomplet::aftocomplet(QTextEdit *editor, QObject *parent)
    : editor(editor)
{
    Q_UNUSED(parent);
    QStringList keywords = {
        "int", "float", "if", "else", "while", "return", "class",
        "import", "start", "print", "function", "void", "break", "delete"};
}