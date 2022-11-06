#ifndef _ASQT_EQTAB_
#define _ASQT_EQTAB_

#include "ASQt_Define.h"

class ASQt_EQTab :public QWidget {
public:
	ASQt_EQTab(std::weak_ptr<Ui::ASQt_SourceClass> _parent, std::weak_ptr<AS::Equalizer> _effect);
	~ASQt_EQTab();

private:
	std::weak_ptr<Ui::ASQt_SourceClass> m_wpUi;
	std::weak_ptr<AS::Equalizer> m_wpEffect;

	AS::EQParam m_Param{};
private slots:
	void ChangeParam();
	void ChangeEnable();
};

#endif
