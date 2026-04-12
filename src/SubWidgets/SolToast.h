// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTOAST_H
#define SOLTOAST_H


class QWidget;
class QString;

class SolToast
{
public:
    /**
     * 토스트 메시지를 띄웁니다.
     * \a inToastParent가 있다면 해당 위젯 내부 상단에,
     * 없다면 주 화면 우측 하단에 표시됩니다.
     * 토스트 메시지는 나온 곳으로 사라집니다.
     *
     * @param inMessage Message
     * @param inToastParent If it's nullptr, located in the bottom right corner of the main screen.
     * @param inDurationMsec Message display duration. msec
     */
    static void showToast(const QString& inMessage, QWidget* inToastParent, const int inDurationMsec = 1'500);

    /**
     * 토스트 메시지를 띄웁니다.
     * \a inTargetWidget 의 외부 상단에 표기됩니다.
     * 토스트 메시지는 위쪽으로 사라집니다.
     *
     * @param inMessage Message
     * @param inTargetWidget A widget that provides the reference position of the message.
     * @param inDurationMsec Message display duration. msec
     */
    static void popToastOnWidget(const QString& inMessage, QWidget* inTargetWidget, const int inDurationMsec = 500);
};


#endif //SOLTOAST_H
