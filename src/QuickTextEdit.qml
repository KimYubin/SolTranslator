import QtQuick

Rectangle {
    anchors.centerIn: parent
    width: 340
    color: "#2d2d2d"
    TextEdit {
        anchors.fill: parent

        id: textItem
        text: "안녕하세요, Qt Quick 폰트 렌더링입니다!"
        padding: 10
        wrapMode: TextEdit.WrapAtWordBoundaryOrAnywhere
        // renderType: Text.NativeRendering
        color: "#fafafa"
        
        font {
            family: 'Noto Sans KR'
            pointSize: 12
        }
    }

}