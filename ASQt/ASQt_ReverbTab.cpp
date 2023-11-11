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

		connect(ui->m_APFDelayTime0, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF0);
		connect(ui->m_APFDecayLevel0, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF0);
		connect(ui->m_EnableAPF0, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_APF0);

		connect(ui->m_APFDelayTime1, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF1);
		connect(ui->m_APFDecayLevel1, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF1);
		connect(ui->m_EnableAPF1, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_APF1);

		connect(ui->m_WetSlider, &QSlider::sliderReleased, this, &ASQt_ReverbTab::ChangeParam_WetDryMix);
		connect(ui->m_WetSlider, &QSlider::sliderMoved, this, [this] {
			if (auto ui = m_wpUi.lock()) {
				ui->m_WetValue->setText(QString::number(ui->m_WetSlider->value() / 100.0f, 'f', 2));
			}
			});
		connect(ui->m_DrySlider, &QSlider::sliderReleased, this, &ASQt_ReverbTab::ChangeParam_WetDryMix);
		connect(ui->m_DrySlider, &QSlider::sliderMoved, this, [this] {
			if (auto ui = m_wpUi.lock()) {
				ui->m_DryValue->setText(QString::number(ui->m_DrySlider->value() / 100.0f, 'f', 2));
			}
			});

		connect(ui->m_EnableReverb, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_Reverb);

		std::array<AS::CombParam, 4> combParam;
		combParam[0] = AS::CombParam(39.0f, 0.835603f);
		combParam[1] = AS::CombParam(36.7f, 0.844501f);
		combParam[2] = AS::CombParam(33.2f, 0.858223f);
		combParam[3] = AS::CombParam(29.9f, 0.832147f);

		std::array<AS::AllpassParam, 2> apfParam;
		apfParam[0] = AS::AllpassParam(5.0, 0.7);
		apfParam[1] = AS::AllpassParam(1.7, 0.7);

		std::array<bool, 4> combEnable{ true, true, true, true };
		std::array<bool, 2> apfEnable{ true, true };
		m_Param = AS::ReverbParam{
			combParam,
			apfParam,
			combEnable,
			apfEnable,
			1.0f,
			0.25f
		};
		if (auto rev = m_wpEffect.lock())
			rev->SetEffectParam(m_Param);
		if (auto ui = m_wpUi.lock()) {
			ui->m_DelayTime0->setValue(m_Param.combParams[0].delayTime);
			ui->m_DelayTime1->setValue(m_Param.combParams[1].delayTime);
			ui->m_DelayTime2->setValue(m_Param.combParams[2].delayTime);
			ui->m_DelayTime3->setValue(m_Param.combParams[3].delayTime);
			ui->m_Feedback0->setValue(m_Param.combParams[0].decayLevel);
			ui->m_Feedback1->setValue(m_Param.combParams[1].decayLevel);
			ui->m_Feedback2->setValue(m_Param.combParams[2].decayLevel);
			ui->m_Feedback3->setValue(m_Param.combParams[3].decayLevel);
			ui->m_EnableComb0->setCheckState(m_Param.combEnable[0] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb1->setCheckState(m_Param.combEnable[1] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb2->setCheckState(m_Param.combEnable[2] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb3->setCheckState(m_Param.combEnable[3] ? Qt::Checked : Qt::Unchecked);

			ui->m_APFDelayTime0->setValue(m_Param.apfParams[0].delayTime);
			ui->m_APFDelayTime1->setValue(m_Param.apfParams[1].delayTime);
			ui->m_APFDecayLevel0->setValue(m_Param.apfParams[0].decayLevel);
			ui->m_APFDecayLevel1->setValue(m_Param.apfParams[1].decayLevel);
			ui->m_EnableAPF0->setCheckState(m_Param.apfEnable[0] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableAPF1->setCheckState(m_Param.apfEnable[1] ? Qt::Checked : Qt::Unchecked);

			ui->m_WetSlider->setValue(m_Param.wet);
			ui->m_WetValue->setText(QString::number(m_Param.wet, 'f', 2));

			ui->m_DrySlider->setValue(m_Param.wet);
			ui->m_DryValue->setText(QString::number(m_Param.wet, 'f', 2));
		}
	}
}

ASQt_ReverbTab::~ASQt_ReverbTab() {
}

void ASQt_ReverbTab::ChangeParam_Comb0() {
	AS::CombParam& combParam = m_Param.combParams.at(0);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime0->value(), ui->m_Feedback0->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb1() {
	AS::CombParam& combParam = m_Param.combParams.at(1);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime1->value(), ui->m_Feedback1->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb2() {
	AS::CombParam& combParam = m_Param.combParams.at(2);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime2->value(), ui->m_Feedback2->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb3() {
	AS::CombParam& combParam = m_Param.combParams.at(3);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime3->value(), ui->m_Feedback3->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_APF0() {
	auto delay = m_Param.apfParams[0].delayTime;
	auto decay = m_Param.apfParams[0].decayLevel;
	if (auto ui = m_wpUi.lock()) {
		delay = ui->m_APFDelayTime0->value();
		decay = ui->m_APFDecayLevel0->value();
	}
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_APF1() {
	auto delay = m_Param.apfParams[1].delayTime;
	auto decay = m_Param.apfParams[1].decayLevel;
	if (auto ui = m_wpUi.lock()) {
		delay = ui->m_APFDelayTime1->value();
		decay = ui->m_APFDecayLevel1->value();
	}
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_WetDryMix() {
	auto& dry = m_Param.dry;
	auto& wet = m_Param.wet;
	if (auto ui = m_wpUi.lock()) {
		dry = ui->m_DrySlider->value() / 100.0f;
		ui->m_DryValue->setText(QString::number(dry, 'f', 2));
		wet = ui->m_WetSlider->value() / 100.0f;
		ui->m_WetValue->setText(QString::number(wet, 'f', 2));
	}
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_Comb0() {
	auto& enable = m_Param.combEnable.at(0);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableComb0->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_Comb1() {
	auto& enable = m_Param.combEnable.at(1);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableComb1->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_Comb2() {
	auto& enable = m_Param.combEnable.at(2);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableComb2->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_Comb3() {
	auto& enable = m_Param.combEnable.at(3);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableComb3->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_APF0() {
	auto& enable = m_Param.apfEnable.at(0);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableAPF0->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_APF1() {
	auto& enable = m_Param.apfEnable.at(1);
	if (auto ui = m_wpUi.lock())
		enable = ui->m_EnableAPF1->checkState();
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeEnable_Reverb() {
	auto ui = m_wpUi.lock();
	if (auto rev = m_wpEffect.lock()) {
		rev->SetEnable(ui->m_EnableReverb->checkState() == Qt::CheckState::Checked);
	}
}