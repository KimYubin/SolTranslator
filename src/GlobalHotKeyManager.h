//
// Created by YubinKim on 25/03/13 목.
//

#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H
#include <QObject>


enum class HotkeyType;
class QHotkey;

class GlobalHotKeyManager : public QObject
{
    Q_OBJECT

public:
    GlobalHotKeyManager(QObject* parent = nullptr);


private slots:
    void RegisterHotKey(HotkeyType InHotkey, const QKeySequence& shortcut, std::function<void(GlobalHotKeyManager*)> InFunction);

public:
    void FireSimpleTranslate();

private:
    std::unordered_map<HotkeyType, QHotkey* > hotKeys;
    
};

enum class HotkeyType
{
    None,
    
    simpleTranslate,
    
    size
};


#endif //GLOBALHOTKEYMANAGER_H
