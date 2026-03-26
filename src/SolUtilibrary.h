// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLUTILIBRARY_H
#define SOLUTILIBRARY_H
#include <QRect>
#include "../../external/magic_enum.hpp"

/**
 * style sheet을 통한 값 변경을 위한 qproperty helper 입니다.
 * 
 * QWidget을 상속하는 클래스에서 사용할 수 있습니다.
 * 
 * style sheet에서 다음 예시의 형태로 사용할 수 있습니다.
 * @code
 * HistoryListView
 * {
 *     qproperty-<varName> : ${value};
 *     qproperty-itemTextColor: rgba(255, 255, 255, 0.9);
 * }
 * @endcode 
 * 
 * @param type: 변수 타입 
 * @param varName: 변수 이름 
 * 
 */
#define SOL_QSS_TYPE(type, varName) \
private: \
    Q_PROPERTY(type varName READ get##varName WRITE set##varName) \
    void set##varName(const type& inColor) { _##varName = inColor; }; \
    type get##varName() const { return _##varName; }; \
    type _##varName;


#define SOL_QSS_COLOR(colorName) SOL_QSS_TYPE(QColor, colorName)


class QObject;
class QWidget;

namespace Sol
{
/**
 * Other QObject와 그 부모가 this인지 재귀적으로 확인합니다.
 * @param inThis 비교 주체
 * @param inOther 비교 대상. 부모 객체도 확인합니다.
 * @return 
 */
bool isThis(const QObject* inThis, const QObject* inOther);


/** QWidget 기본 QFont 셋팅을 유지하면서 PreferNoHinting으로 설정합니다. */
void noHintingFont(QWidget* inOutWidget);

/** 앱 전체 기본 QFont 셋팅을 유지하면서 PreferNoHinting으로 설정합니다. */
void noHintingFont();


/**
 * 지정된 point의 screen의 유효한 영역을 반환합니다.
 * 어떤 screen도 해당되지 않으면, primaryScreen을 시도하고,
 * 실패시 QRect()을 반홥합니다.
 * 
 * @param inPoint 
 * @return 
 */
QRect availableGeometryAt(const QPoint& inPoint);


/**
 * innerRect을 outerRect 안으로 이동시킵니다.
 * 
 * @param outerRect  
 * @param innerRect 
 * @return 
 */
[[nodiscard]] QRect moveToInside(const QRect& outerRect, const QRect& innerRect);


/**
 * std::string_view를 QString으로 변환합니다.
 */
QString qStrFromStdView(const std::string_view& inStrView);


/**
 * enum 열거자의 이름을 QString으로 변환합니다.
 */
template <typename EnumType>
    requires std::is_enum_v<EnumType>
QString enumToQStr(const EnumType inDefaultVal)
{
    return qStrFromStdView(magic_enum::enum_name<EnumType>(inDefaultVal));
}

/**
 * QString을 enum 열거자로 변환합니다.
 * @param inString 출발 string
 * @param inDefaultVal string이 유효하지 않을 경우 사용할 기본 타입
 */
template <typename EnumType>
    requires std::is_enum_v<EnumType>
EnumType qStrToEnum(const QString& inString, const EnumType inDefaultVal)
{
    return magic_enum::enum_cast<EnumType>(inString.toStdString()).value_or(inDefaultVal);
}

} // namespace Sol



#endif //SOLUTILIBRARY_H
