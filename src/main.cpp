#include <iostream>
#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QInputDialog> // <-- Added this line
#include <QKeyEvent>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QSettings>
#include <QTextEdit>
#include <QTextStream>
#include <QToolBar>
#include <QTreeView>
#include "aftocomplet.h"
#include "keypresshandler.h"
#include "syntaxhighlighter.h"
#include <fstream>
#include <string>

class CodeEditor : public QMainWindow
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        editor = new QTextEdit(this);
        setCentralWidget(editor);

        highlighter = new SyntaxHighlighter(editor->document());

        keyPressHandler = new KeyPressHandler(editor, this);
        connect(keyPressHandler, &KeyPressHandler::saveRequested, this, &CodeEditor::saveFile);
        connect(keyPressHandler, &KeyPressHandler::undoRequested, editor, &QTextEdit::undo);

        autoComplete = new aftocomplet(editor, this);

        QMenu *fileMenu = menuBar()->addMenu("Файл");
        QAction *newFile = fileMenu->addAction("Новый файл");
        QAction *openFile = fileMenu->addAction("Открыть файл");
        QAction *openFolder = fileMenu->addAction("Открыть папку");
        QAction *saveFile = fileMenu->addAction("Сохранить файл");
        QAction *setings = fileMenu->addAction("Настройки");

        connect(newFile, &QAction::triggered, this, &CodeEditor::createNewFile);
        connect(openFile, &QAction::triggered, this, &CodeEditor::openFile);
        connect(openFolder, &QAction::triggered, this, &CodeEditor::openFolder);
        connect(saveFile, &QAction::triggered, this, &CodeEditor::saveFile);
        connect(setings, &QAction::triggered, this, &CodeEditor::Setings); // <-- Fixed slot name

        fileTreeDock = new QDockWidget("Файлы", this);
        fileTree = new QTreeView(fileTreeDock);
        fileModel = new QFileSystemModel(this);
        fileTree->setModel(fileModel);
        fileTreeDock->setWidget(fileTree);
        addDockWidget(Qt::LeftDockWidgetArea, fileTreeDock);

        connect(fileTree, &QTreeView::doubleClicked, this, &CodeEditor::openFileFromTree);

        terminal = new QPlainTextEdit(this);
        terminal->setReadOnly(false);
        terminal->setPlaceholderText("Введите команду и нажмите Enter...");
        QDockWidget *terminalDock = new QDockWidget("Терминал", this);
        terminalDock->setWidget(terminal);
        addDockWidget(Qt::BottomDockWidgetArea, terminalDock);

        connect(terminal, &QPlainTextEdit::textChanged, this, &CodeEditor::onTerminalTextChanged);

        QToolBar *toolBar = addToolBar("Инструменты");
        QAction *buildAction = toolBar->addAction("Собрать");
        QAction *runAction = toolBar->addAction("Запустить");

        connect(buildAction, &QAction::triggered, this, &CodeEditor::buildProject);
        connect(runAction, &QAction::triggered, this, &CodeEditor::runProject);

        loadLastFolder();
    }

private slots:
    void onTerminalTextChanged()
    {
        QString text = terminal->toPlainText();
        if (text.endsWith("\n"))
        {                                     // Проверяем, нажата ли клавиша Enter
            QString command = text.trimmed(); // Убираем лишние пробелы и символы новой строки
            terminal->clear();                // Очищаем терминал после ввода команды
            processCommand(command);          // Обрабатываем команду
        }
    }

    void processCommand(const QString &command)
    {

        if (command == "help")
        {
            terminal->appendPlainText("Доступные команды:\n"
                                      "  help - показать список команд\n"
                                      "  start theme dark - включить тёмную тему\n"
                                      "  start theme light - включить светлую тему\n"
                                      "  start theme dark blue - включить синюю тёмную тему\n"
                                      "  start theme dracula - включить тему Dracula\n"
                                      "  build - собрать проект\n"
                                      "  run - запустить проект\n"
                                      "  clear - очистить терминал\n");
        }
        else if (command == "start theme dark")
        {
            applyDarkTheme();
            terminal->appendPlainText("Тёмная тема активирована.");
        }
        else if (command == "start theme light")
        {
            applyLightTheme();
            terminal->appendPlainText("Светлая тема активирована.");
        }
        else if (command == "start theme dark blue")
        {
            applyDarkBlueTheme();
            terminal->appendPlainText("Синяя тёмная тема активирована.");
        }
        else if (command == "start theme dracula")
        {
            applyDraculaTheme();
            terminal->appendPlainText("Тема Dracula активирована.");
        }
        else if (command == "build")
        {
            buildProject();
            terminal->appendPlainText("Проект собран.");
        }
        else if (command == "run")
        {
            runProject();
            terminal->appendPlainText("Проект запущен.");
        }
        else if (command == "clear")
        {
            terminal->clear();
        }
        else
        {
            terminal->appendPlainText("Неизвестная команда. Введите 'help' для списка команд.");
        }
    }

    void createNewFile()
    {
        editor->clear();
        currentFile.clear();
    }

    void openFile()
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Все файлы (*.*)");
        if (fileName.isEmpty())
            return;

        loadFile(fileName);
    }

    void saveFile()
    {
        QString fileName = currentFile.isEmpty()
                               ? QFileDialog::getSaveFileName(this, "Сохранить файл", "", "Все файлы (*.*)")
                               : currentFile;
        if (fileName.isEmpty())
            return;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
            return;
        }

        QTextStream out(&file);
        out << editor->toPlainText();
        file.close();
        currentFile = fileName;
    }

    void openFolder()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "Открыть папку");
        if (dir.isEmpty())
            return;

        QSettings settings("PablaIDE", "CodeEditor");
        settings.setValue("lastFolderPath", dir);

        fileModel->setRootPath(dir);
        fileTree->setRootIndex(fileModel->index(dir));
        currentFolder = dir; // <-- Make sure to set currentFolder
    }

    void loadLastFolder()
    {
        QSettings settings("PablaIDE", "CodeEditor");
        QString lastPath = settings.value("lastFolderPath", "").toString();
        if (!lastPath.isEmpty() && QDir(lastPath).exists())
        {
            fileModel->setRootPath(lastPath);
            fileTree->setRootIndex(fileModel->index(lastPath));
            currentFolder = lastPath; // <-- Make sure to set currentFolder
        }
    }

    void openFileFromTree(const QModelIndex &index)
    {
        QFileInfo fileInfo = fileModel->fileInfo(index);
        if (fileInfo.isFile())
        {
            loadFile(fileInfo.filePath());
        }
    }

    void Setings() // <-- Slot name fixed
    {
        QString theme;
        theme = QInputDialog::getItem(this, "Выберите тему", "Выберите тему:",
                                      {"Тёмная тема", "Светлая тема", "Синяя тёмная тема", "Тема Dracula"}, 0, false);

        if (theme == "Тёмная тема")
        {
            applyDarkTheme();
        }
        else if (theme == "Светлая тема")
        {
            applyLightTheme();
        }
        else if (theme == "Синяя тёмная тема")
        {
            applyDarkBlueTheme();
        }
        else if (theme == "Тема Dracula")
        {
            applyDraculaTheme();
        }

        QString language;
        language = QInputDialog::getItem(this, "Выберите язык", "Выберите язык:",
                                         {"Русский", "Английский"}, 0, false);
        if (language == "Русский")
        {
            // setLanguage("Russian");
        }
        else if (language == "Английский")
        {
            // setLanguage("English");
        }

        terminal->appendPlainText("Настройки сохранены.");


        QSettings settings("PablaIDE", "CodeEditor");
        settings.setValue("lastFolderPath", currentFolder); // <-- Save current folder
        settings.setValue("theme", theme);
        terminal->appendPlainText("Настройки сохранены.");
        loadLastFolder(); // <-- Load last folder
    }

    void loadFile(const QString &fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
            return;
        }

        QTextStream in(&file);
        editor->setText(in.readAll());
        file.close();
        currentFile = fileName;
    }

    void buildProject()
    {
        if (currentFolder.isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Сначала откройте папку проекта.");
            return;
        }

        QProcess *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, this, [this, process]()
                { terminal->appendPlainText(process->readAllStandardOutput()); });
        connect(process, &QProcess::readyReadStandardError, this, [this, process]()
                { terminal->appendPlainText(process->readAllStandardError()); });

        terminal->appendPlainText("Сборка проекта...");
        process->setWorkingDirectory(currentFolder);
        process->start("cmake --build .");
    }

    void runProject()
    {
        if (currentFolder.isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Сначала откройте папку проекта.");
            return;
        }

        QProcess *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, this, [this, process]()
                { terminal->appendPlainText(process->readAllStandardOutput()); });
        connect(process, &QProcess::readyReadStandardError, this, [this, process]()
                { terminal->appendPlainText(process->readAllStandardError()); });

        terminal->appendPlainText("Запуск проекта...");
        process->setWorkingDirectory(currentFolder);
        process->start("./untitled20");
    }

    void applyDarkTheme()
    {
        qApp->setStyleSheet("QTextEdit { background-color: #2b2b2b; color: #ffffff; }"
                            "QTreeView { background-color: #2b2b2b; color: #ffffff; }"
                            "QPlainTextEdit { background-color: #2b2b2b; color: #ffffff; }");
    }

    void applyLightTheme()
    {
        qApp->setStyleSheet("QTextEdit { background-color: #ffffff; color: #000000; }"
                            "QTreeView { background-color: #ffffff; color: #000000; }"
                            "QPlainTextEdit { background-color: #ffffff; color: #000000; }");
    }

    void applyDarkBlueTheme()
    {
        qApp->setStyleSheet("QTextEdit { background-color: #1e1e2f; color: #dcdcdc; }"
                            "QTreeView { background-color: #1e1e2f; color: #dcdcdc; }"
                            "QPlainTextEdit { background-color: #1e1e2f; color: #dcdcdc; }");
    }

    void applyDraculaTheme()
    {
        qApp->setStyleSheet("QTextEdit { background-color:rgb(14, 0, 86); color:rgb(255, 255, 255); }"
                            "QTreeView { background-color:rgb(14, 0, 86); color:rgb(255, 255, 255); }"
                            "QPlainTextEdit { background-color:rgb(14, 0, 86); color:rgb(255, 255, 255); }");
    }

private:
    QTextEdit *editor;
    SyntaxHighlighter *highlighter;
    KeyPressHandler *keyPressHandler;
    aftocomplet *autoComplete;
    QDockWidget *fileTreeDock;
    QTreeView *fileTree;
    QFileSystemModel *fileModel;
    QPlainTextEdit *terminal;
    QString currentFile;
    QString currentFolder;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CodeEditor editor;
    editor.setWindowTitle("Pabla IDE");
    editor.resize(1000, 600);
    editor.show();
    return app.exec();
}

#include "main.moc"