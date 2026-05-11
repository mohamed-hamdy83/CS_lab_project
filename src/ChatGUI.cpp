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
    
// --- NEW MULTI-SELECT ONLINE USERS LIST ---
    onlineUsersList = new QListWidget();
    onlineUsersList->setSelectionMode(QAbstractItemView::MultiSelection);
    onlineUsersList->setMaximumHeight(60); // Keeps it compact like a thick dropdown
    onlineUsersList->setMaximumWidth(150);
    onlineUsersList->setToolTip("Click multiple names for a group chat. Deselect all to broadcast.");
    
    messageInput = new QLineEdit();
    messageInput->setPlaceholderText("type a message...");
    sendButton = new QPushButton("Send");
    
    inputLayout->addWidget(onlineUsersList);
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    settingsButton = new QPushButton("Settings"); 
    chatLayout->addWidget(settingsButton); 
    chatLayout->addWidget(chatHistory);
    chatLayout->addLayout(inputLayout);

    // Update the button click to grab all selected names
    connect(sendButton, &QPushButton::clicked, this, [this]() {
        QStringList selectedUsers;
        for (QListWidgetItem* item : onlineUsersList->selectedItems()) {
            selectedUsers << item->text();
        }
        
        // We join the selected names with commas to reuse our group chat logic!
        emit sendMessageRequested(messageInput->text(), selectedUsers.join(","));
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

void ChatGUI::setWindowTitleByUsername(const QString& username) {
    this->setWindowTitle("Chat App - " + username);
}

void ChatGUI::updateOnlineUsers(const QStringList& users, const QString& currentUsername) {
    onlineUsersList->clear();
    for (const QString& user : users) {
        // Only add the user to the list if it's not us! 
        if (user != currentUsername && user != "Unknown") {
            onlineUsersList->addItem(user);
        }
    }
}