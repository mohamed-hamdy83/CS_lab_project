#ifndef CHAT_GUI_H
#define CHAT_GUI_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QStackedWidget>
#include <QString>
#include <QMessageBox>
#include <QListWidget>

class ChatGUI : public QWidget {
    Q_OBJECT // this macro is required to use qt signals and slots

public:
    explicit ChatGUI(QWidget *parent = nullptr);

    // methods for the controller to update the gui
    void showLoginScreen();
    void showChatScreen();
    void showErrorPopup(const QString& errorMessage);
    void appendChatMessage(const QString& sender, const QString& message);
    void setWindowTitleByUsername(const QString& username);
    void updateOnlineUsers(const QStringList& users, const QString& currentUsername);

signals:
    // the gui does NOT process the login or send the message itself.
    void loginRequested(const QString& username);
void sendMessageRequested(const QString& message, const QString& recipient);

private:

    QListWidget* onlineUsersList;

    // the container that holds the different screens
    QStackedWidget* stackedWidget;

    // screen 1: login
    QWidget* loginWidget;
    QLineEdit* usernameInput;
    QPushButton* loginButton;

    // screen 2: chat
    QWidget* chatWidget;
    QTextEdit* chatHistory;
    QLineEdit* messageInput;
    QPushButton* sendButton;
    QPushButton* settingsButton;

    // screen 3: settings
    QWidget* settingsWidget;
    QPushButton* backButton;     

    // helper function to build the ui
    void setupUI();
};

#endif