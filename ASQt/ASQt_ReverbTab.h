#ifndef _ASQT_REVERBTAB_
#define _ASQT_REVERBTAB_

#include "ASQt_Define.h"

class ASQt_ReverbTab :public QWidget {
public:
	ASQt_ReverbTab(std::weak_ptr<Ui::ASQt_SourceClass> _parent, std::weak_ptr<AS::Reverb> _effect);
	~ASQt_ReverbTab();

private:
	std::weak_ptr<Ui::ASQt_SourceClass> m_wpUi;
	std::weak_ptr<AS::Equalizer> m_wpEffect;

	AS::ReverbParam m_Param{};
private slots:
	void ChangeParam_Comb0();
	void ChangeParam_Comb1();
	void ChangeParam_Comb2();
	void ChangeParam_Comb3();
	void ChangeParam_APF0();
	void ChangeParam_APF1();
	void ChangeParam_WetDryMix();

	void ChangeEnable_Comb0();
	void ChangeEnable_Comb1();
	void ChangeEnable_Comb2();
	void ChangeEnable_Comb3();
	void ChangeEnable_APF0();
	void ChangeEnable_APF1();
	void ChangeEnable_Reverb();
};

#endif