#include "ASQt_ReverbTab.h"

ASQt_ReverbTab::ASQt_ReverbTab(std::weak_ptr<Ui::ASQt_SourceClass> _parent, std::weak_ptr<AS::Reverb> _effect) :m_wpUi(_parent), m_wpEffect(_effect) {
	if (auto ui = m_wpUi.lock()) {
		connect(ui->m_DelayTime0, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb0);
		connect(ui->m_Feedback0, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb0);
		connect(ui->m_EnableComb0, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Comb0);

		connect(ui->m_DelayTime1, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb1);
		connect(ui->m_Feedback1, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb1);
		connect(ui->m_EnableComb1, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Comb1);

		connect(ui->m_DelayTime2, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb2);
		connect(ui->m_Feedback2, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb2);
		connect(ui->m_EnableComb2, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Comb2);

		connect(ui->m_DelayTime3, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb3);
		connect(ui->m_Feedback3, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_Comb3);
		connect(ui->m_EnableComb3, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Comb3);

		connect(ui->m_APF0FreqSlider, &QSlider::sliderReleased, this, &ASQt_ReverbTab::ChangeParam_APF0);
		connect(ui->m_APF0QValue, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF0);
		connect(ui->m_EnableAPF0, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_APF0);

		connect(ui->m_APF1FreqSlider, &QSlider::sliderReleased, this, &ASQt_ReverbTab::ChangeParam_APF1);
		connect(ui->m_APF1QValue, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF1);
		connect(ui->m_EnableAPF1, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_APF1);

		connect(ui->m_EnableReverb, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Reverb);
	}
}

ASQt_ReverbTab::~ASQt_ReverbTab() {
}

void ASQt_ReverbTab::ChangeParam_Comb0() {
}

void ASQt_ReverbTab::ChangeParam_Comb1() {
}

void ASQt_ReverbTab::ChangeParam_Comb2() {
}

void ASQt_ReverbTab::ChangeParam_Comb3() {
}

void ASQt_ReverbTab::ChangeParam_APF0() {
}

void ASQt_ReverbTab::ChangeParam_APF1() {
}

void ASQt_ReverbTab::ChangeParam_WetDryMix() {
}

void ASQt_ReverbTab::ChangeEnable_Comb0() {
}

void ASQt_ReverbTab::ChangeEnable_Comb1() {
}

void ASQt_ReverbTab::ChangeEnable_Comb2() {
}

void ASQt_ReverbTab::ChangeEnable_Comb3() {
}

void ASQt_ReverbTab::ChangeEnable_APF0() {
}

void ASQt_ReverbTab::ChangeEnable_APF1() {
}

void ASQt_ReverbTab::ChangeEnable_Reverb() {
}