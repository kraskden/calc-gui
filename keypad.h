#ifndef KEYPAD_H
#define KEYPAD_H

#include <QHash>
#include <QWidget>
#include <QSignalMapper>

class QPushButton;

class Keypad : public QWidget {
    Q_OBJECT

public:
    enum Button {
        Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
        KeyEquals, KeyPlus, KeyMinus, KeyTimes, KeyDivide,
        KeyRadixChar, KeyClear, KeyEE, KeyLeftPar, KeyRightPar,
        KeyRaise, KeySqrt, KeyLg, KeyBS, KeyPi, KeyAns,
        KeyX, KeyXEquals, KeyExp, KeyLn, KeySin, KeyAsin, KeyCos,
        KeyAcos, KeyTan, KeyAtan
    };

    explicit Keypad(QWidget* parent = 0);

signals:
    void buttonPressed(Keypad::Button) const;

public slots:
    void handleRadixCharacterChange();
    void retranslateText();

protected slots:
    void emitButtonPressed(int button) const;

protected:
    virtual void changeEvent(QEvent*);

private:
    Q_DISABLE_COPY(Keypad)

    QPushButton* key(Button button) const;
    void createButtons();
    void disableButtonFocus();
    void layoutButtons();
    void setButtonTooltips();
    void sizeButtons();

    static const struct KeyDescription {
        QString label;
        Button button;
        bool boldFont;
        int gridRow;
        int gridColumn;
    } keyDescriptions[];

    QHash<Button, QPair<QPushButton*, const KeyDescription*> > keys;
    QSignalMapper mapper;

};

#endif // KEYPAD_H
