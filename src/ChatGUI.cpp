#include "ChatGUI.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ChatGUI::ChatGUI(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ChatGUI::setupUI() {
    stackedWidget = new QStackedWidget(this);

    // --- build screen 1: login ---
    loginWidget = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginWidget);
    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("enter username...");
    loginButton = new QPushButton("Connect");
    loginLayout->addWidget(new QLabel("welcome to the chat app"));
    loginLayout->addWidget(usernameInput);
    loginLayout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        emit loginRequested(usernameInput->text());
    });

    // --- build screen 2: chat ---
    chatWidget = new QWidget();
    QVBoxLayout* chatLayout = new QVBoxLayout(chatWidget);
    chatHistory = new QTextEdit();
    chatHistory->setReadOnly(true); 
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    messageInput = new QLineEdit();
    messageInput->setPlaceholderText("type a message...");
    sendButton = new QPushButton("Send");
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    settingsButton = new QPushButton("Settings"); 

    chatLayout->addWidget(settingsButton); 
    chatLayout->addWidget(chatHistory);
    chatLayout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, [this]() {
        emit sendMessageRequested(messageInput->text());
        messageInput->clear();
    });

    // --- build screen 3: settings ---
    settingsWidget = new QWidget();
    QVBoxLayout* settingsLayout = new QVBoxLayout(settingsWidget);
    settingsLayout->addWidget(new QLabel("Settings Menu"));
    settingsLayout->addWidget(new QLabel("Theme: Dark Mode (Coming Soon)"));
    settingsLayout->addWidget(new QLabel("Notifications: On"));
    
    backButton = new QPushButton("Back to Chat");
    settingsLayout->addWidget(backButton);
    settingsLayout->addStretch(); // pushes everything to the top

    // --- connect navigation buttons ---
    connect(settingsButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(settingsWidget);
    });
    connect(backButton, &QPushButton::clicked, this, [this]() {
        stackedWidget->setCurrentWidget(chatWidget);
    });

    // add all screens to the stack
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(chatWidget);
    stackedWidget->addWidget(settingsWidget);

    // main layout for the whole window
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}

// --- methods for the logic controller to call ---

void ChatGUI::showLoginScreen() {
    stackedWidget->setCurrentWidget(loginWidget);
}

void ChatGUI::showChatScreen() {
    stackedWidget->setCurrentWidget(chatWidget);
}

// screen 3: error popup 
void ChatGUI::showErrorPopup(const QString& errorMessage) {
    QMessageBox::critical(this, "Network Error", errorMessage);
}

void ChatGUI::appendChatMessage(const QString& sender, const QString& message) {
    chatHistory->append("<b>" + sender + ":</b> " + message);
}