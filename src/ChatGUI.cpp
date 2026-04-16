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

    // when login is clicked, emit the signal with the text inside the input
    connect(loginButton, &QPushButton::clicked, this, [this]() {
        emit loginRequested(usernameInput->text());
    });

    // --- build screen 2: chat ---
    chatWidget = new QWidget();
    QVBoxLayout* chatLayout = new QVBoxLayout(chatWidget);
    chatHistory = new QTextEdit();
    chatHistory->setReadOnly(true); // users shouldn't type in the history box
    
    QHBoxLayout* inputLayout = new QHBoxLayout();
    messageInput = new QLineEdit();
    messageInput->setPlaceholderText("type a message...");
    sendButton = new QPushButton("Send");
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);
    
    chatLayout->addWidget(chatHistory);
    chatLayout->addLayout(inputLayout);

    // when send is clicked, emit the signal and clear the input box
    connect(sendButton, &QPushButton::clicked, this, [this]() {
        emit sendMessageRequested(messageInput->text());
        messageInput->clear();
    });

    // add screens to the stack
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(chatWidget);

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