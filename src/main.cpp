#include <iostream>
#include <QApplication>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QInputDialog> // <-- Added this line
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
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
#include "thememanager.h"
#include "languagemanager.h"
#include <fstream>
#include <string>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(ThemeManager *themeManager, LanguageManager *languageManager, QWidget *parent = nullptr)
        : QDialog(parent), m_themeManager(themeManager), m_languageManager(languageManager)
    {
        setWindowTitle(m_languageManager->tr("settings_title", "dialogs"));
        setModal(true);
        resize(400, 300);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // Секция выбора темы
        QLabel *themeLabel = new QLabel(m_languageManager->tr("theme_label", "dialogs"), this);
        mainLayout->addWidget(themeLabel);

        m_themeComboBox = new QComboBox(this);
        QStringList themes = m_themeManager->getAvailableThemes();
        for (const QString &theme : themes) {
            QString description = m_themeManager->getThemeDescription(theme);
            if (!description.isEmpty()) {
                m_themeComboBox->addItem(theme + " - " + description, theme);
            } else {
                m_themeComboBox->addItem(theme, theme);
            }
        }
        mainLayout->addWidget(m_themeComboBox);

        // Секция выбора языка
        QLabel *languageLabel = new QLabel(m_languageManager->tr("language_label", "dialogs"), this);
        mainLayout->addWidget(languageLabel);

        m_languageComboBox = new QComboBox(this);
        QStringList languages = m_languageManager->getAvailableLanguages();
        for (const QString &lang : languages) {
            QString name = m_languageManager->getLanguageName(lang);
            m_languageComboBox->addItem(name, lang);
        }
        mainLayout->addWidget(m_languageComboBox);

        // Кнопки
        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        mainLayout->addWidget(buttonBox);

        // Предварительный просмотр темы
        connect(m_themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &SettingsDialog::previewTheme);
    }

    QString getSelectedTheme() const
    {
        return m_themeComboBox->currentData().toString();
    }

    QString getSelectedLanguage() const
    {
        return m_languageComboBox->currentData().toString();
    }

private slots:
    void previewTheme()
    {
        QString themeName = getSelectedTheme();
        if (!themeName.isEmpty()) {
            m_themeManager->applyTheme(themeName);
        }
    }

private:
    ThemeManager *m_themeManager;
    LanguageManager *m_languageManager;
    QComboBox *m_themeComboBox;
    QComboBox *m_languageComboBox;
};

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

        // Инициализация менеджеров
        themeManager = new ThemeManager(this);
        languageManager = new LanguageManager(this);

        // Загружаем сохраненные настройки
        QSettings settings("PablaIDE", "CodeEditor");
        QString savedTheme = settings.value("theme", "").toString();
        QString savedLanguage = settings.value("language", "ru").toString();
        
        if (!savedTheme.isEmpty() && themeManager->themeExists(savedTheme)) {
            themeManager->applyTheme(savedTheme);
        }
        
        if (languageManager->languageExists(savedLanguage)) {
            languageManager->applyLanguage(savedLanguage);
        }
        
        // Обновляем интерфейс с новым языком
        updateInterfaceLanguage();

        loadLastFolder();
    }

private slots:
    void onTerminalTextChanged()
    {
        QString text = terminal->toPlainText();
        if (text.endsWith("\n"))
        {                                     // Проверяем, нажата ли клавиша Enter
            QString command = text.trimmed(); // Убираем лишние пробелы и символы новой строки
            
            // Очищаем терминал перед выполнением команды
            terminal->clear();
            
            processCommand(command);          // Обрабатываем команду
        }
    }

    void processCommand(const QString &command)
    {
        if (command == "help")
        {
            QString helpText = languageManager->tr("help_title", "commands") + "\n"
                              "  " + languageManager->tr("help_help", "commands") + "\n"
                              "  " + languageManager->tr("help_themes", "commands") + "\n"
                              "  " + languageManager->tr("help_start_theme", "commands") + "\n"
                              "  " + languageManager->tr("help_language", "commands") + "\n"
                              "  " + languageManager->tr("help_build", "commands") + "\n"
                              "  " + languageManager->tr("help_run", "commands") + "\n"
                              "  " + languageManager->tr("help_clear", "commands") + "\n";
            
            QStringList themes = themeManager->getAvailableThemes();
            if (!themes.isEmpty()) {
                helpText += "  " + languageManager->tr("available_themes_list", "commands") + "\n";
                for (const QString &theme : themes) {
                    helpText += "    - " + theme + "\n";
                }
            }
            
            terminal->appendPlainText(helpText);
        }
        else if (command == "themes")
        {
            QStringList themes = themeManager->getAvailableThemes();
            if (themes.isEmpty()) {
                terminal->appendPlainText(languageManager->tr("no_themes", "messages"));
            } else {
                terminal->appendPlainText(languageManager->tr("available_themes", "messages"));
                for (const QString &theme : themes) {
                    QString description = themeManager->getThemeDescription(theme);
                    if (!description.isEmpty()) {
                        terminal->appendPlainText("  - " + theme + ": " + description);
                    } else {
                        terminal->appendPlainText("  - " + theme);
                    }
                }
            }
        }
        else if (command == "languages")
        {
            QStringList languages = languageManager->getAvailableLanguages();
            terminal->appendPlainText(languageManager->tr("available_languages", "commands"));
            for (const QString &lang : languages) {
                QString name = languageManager->getLanguageName(lang);
                terminal->appendPlainText("  - " + lang + ": " + name);
            }
        }
        else if (command.startsWith("start theme "))
        {
            QString themeName = command.mid(12); // Убираем "start theme "
            if (themeManager->themeExists(themeName)) {
                themeManager->applyTheme(themeName);
                terminal->appendPlainText(languageManager->tr("theme_activated", QStringList{themeName}, "messages"));
            } else {
                terminal->appendPlainText(languageManager->tr("theme_not_found", QStringList{themeName}, "messages"));
            }
        }
        else if (command.startsWith("language "))
        {
            QString langCode = command.mid(9); // Убираем "language "
            if (languageManager->languageExists(langCode)) {
                languageManager->applyLanguage(langCode);
                updateInterfaceLanguage();
                terminal->appendPlainText(languageManager->tr("language_changed", "messages") + 
                                        languageManager->getLanguageName(langCode));
                
                // Сохраняем настройки
                QSettings settings("PablaIDE", "CodeEditor");
                settings.setValue("language", langCode);
            } else {
                terminal->appendPlainText("Язык '" + langCode + "' не найден. Используйте команду 'languages' для списка доступных языков.");
            }
        }
        else if (command == "build")
        {
            buildProject();
            terminal->appendPlainText(languageManager->tr("project_built", "messages"));
        }
        else if (command == "run")
        {
            runProject();
            terminal->appendPlainText(languageManager->tr("project_started", "messages"));
        }
        else if (command == "clear")
        {
            terminal->clear();
        }
        else
        {
            terminal->appendPlainText(languageManager->tr("unknown_command", "messages"));
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
        QSettings settings("PablaIDE", "CodeEditor");
        QString currentTheme = settings.value("theme", "").toString();
        QString currentLanguage = settings.value("language", "ru").toString();
        
        SettingsDialog dialog(themeManager, languageManager, this);
        
        if (dialog.exec() == QDialog::Accepted)
        {
            QString selectedTheme = dialog.getSelectedTheme();
            QString selectedLanguage = dialog.getSelectedLanguage();
            
            if (!selectedTheme.isEmpty() && themeManager->themeExists(selectedTheme))
            {
                themeManager->applyTheme(selectedTheme);
                terminal->appendPlainText(languageManager->tr("theme_applied", "messages") + selectedTheme);
            }
            
            if (!selectedLanguage.isEmpty() && languageManager->languageExists(selectedLanguage))
            {
                languageManager->applyLanguage(selectedLanguage);
                updateInterfaceLanguage();
                terminal->appendPlainText(languageManager->tr("language_changed", "messages") + 
                                        languageManager->getLanguageName(selectedLanguage));
            }
            
            // Сохраняем настройки
            settings.setValue("lastFolderPath", currentFolder);
            settings.setValue("theme", selectedTheme);
            settings.setValue("language", selectedLanguage);
            terminal->appendPlainText(languageManager->tr("settings_saved", "messages"));
        }
        
        loadLastFolder();
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



private:
    QTextEdit *editor;
    SyntaxHighlighter *highlighter;
    KeyPressHandler *keyPressHandler;
    aftocomplet *autoComplete;
    QDockWidget *fileTreeDock;
    QTreeView *fileTree;
    QFileSystemModel *fileModel;
    QPlainTextEdit *terminal;
    ThemeManager *themeManager;
    LanguageManager *languageManager;
    QString currentFile;
    QString currentFolder;
    
    // Обновление интерфейса при смене языка
    void updateInterfaceLanguage();
};

void CodeEditor::updateInterfaceLanguage()
{
    // Обновляем заголовок окна
    setWindowTitle(languageManager->tr("window_title", "interface"));
    
    // Обновляем меню
    menuBar()->actions().at(0)->setText(languageManager->tr("file_menu", "interface")); // Файл
    menuBar()->actions().at(0)->menu()->actions().at(0)->setText(languageManager->tr("new_file", "interface")); // Новый файл
    menuBar()->actions().at(0)->menu()->actions().at(1)->setText(languageManager->tr("open_file", "interface")); // Открыть файл
    menuBar()->actions().at(0)->menu()->actions().at(2)->setText(languageManager->tr("open_folder", "interface")); // Открыть папку
    menuBar()->actions().at(0)->menu()->actions().at(3)->setText(languageManager->tr("save_file", "interface")); // Сохранить файл
    menuBar()->actions().at(0)->menu()->actions().at(4)->setText(languageManager->tr("settings", "interface")); // Настройки
    
    // Обновляем заголовки доков
    fileTreeDock->setWindowTitle(languageManager->tr("file_tree_title", "interface"));
    
    // Обновляем плейсхолдер терминала
    terminal->setPlaceholderText(languageManager->tr("terminal_placeholder", "interface"));
    
    // Обновляем панель инструментов
    QList<QToolBar*> toolBars = findChildren<QToolBar*>();
    if (!toolBars.isEmpty()) {
        QToolBar* toolbar = toolBars.first();
        if (toolbar->actions().size() >= 2) {
            toolbar->actions().at(0)->setText(languageManager->tr("build", "interface")); // Собрать
            toolbar->actions().at(1)->setText(languageManager->tr("run", "interface")); // Запустить
        }
    }
}

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