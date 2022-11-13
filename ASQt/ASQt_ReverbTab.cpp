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
		connect(ui->m_APF0FreqSlider, &QSlider::sliderMoved, this, [this] { {
				if (auto ui = m_wpUi.lock()) {
					ui->m_APF0FreqValue->setText(QString::number(ui->m_APF0FreqSlider->value()));
				}
			}});
		connect(ui->m_APF0QValue, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF0);
		connect(ui->m_EnableAPF0, &QCheckBox::stateChanged, this, &ASQt_ReverbTab::ChangeEnable_APF0);

		connect(ui->m_APF1FreqSlider, &QSlider::sliderReleased, this, &ASQt_ReverbTab::ChangeParam_APF1);
		connect(ui->m_APF1FreqSlider, &QSlider::sliderMoved, this, [this] { {
				if (auto ui = m_wpUi.lock()) {
					ui->m_APF1FreqValue->setText(QString::number(ui->m_APF1FreqSlider->value()));
				}
			}});
		connect(ui->m_APF1QValue, &QDoubleSpinBox::valueChanged, this, &ASQt_ReverbTab::ChangeParam_APF1);
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
		std::array<bool, 4> combEna{ true, true, true, true };
		std::array<float, 2> apfQ{ 5.05f ,1.65f };
		std::array<bool, 2> apfEna{ true, true };
		m_Param = AS::ReverbParam{
			combParam,
			combEna,
			apfQ,
			apfEna,
			1.0f,
			0.25f
		};
		if (auto rev = m_wpEffect.lock())
			rev->SetEffectParam(m_Param);
		if (auto ui = m_wpUi.lock()) {
			ui->m_DelayTime0->setValue(m_Param.comb[0].delayTime);
			ui->m_DelayTime1->setValue(m_Param.comb[1].delayTime);
			ui->m_DelayTime2->setValue(m_Param.comb[2].delayTime);
			ui->m_DelayTime3->setValue(m_Param.comb[3].delayTime);
			ui->m_Feedback0->setValue(m_Param.comb[0].feedBack);
			ui->m_Feedback1->setValue(m_Param.comb[1].feedBack);
			ui->m_Feedback2->setValue(m_Param.comb[2].feedBack);
			ui->m_Feedback3->setValue(m_Param.comb[3].feedBack);
			ui->m_EnableComb0->setCheckState(m_Param.combEnable[0] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb1->setCheckState(m_Param.combEnable[1] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb2->setCheckState(m_Param.combEnable[2] ? Qt::Checked : Qt::Unchecked);
			ui->m_EnableComb3->setCheckState(m_Param.combEnable[3] ? Qt::Checked : Qt::Unchecked);

			ui->m_APF0FreqSlider->setValue(m_Param.apfFreq[0]);
			ui->m_APF1FreqSlider->setValue(m_Param.apfFreq[1]);
			ui->m_APF0FreqValue->setText(QString::number(m_Param.apfFreq[0]));
			ui->m_APF1FreqValue->setText(QString::number(m_Param.apfFreq[1]));

			ui->m_APF0QValue->setValue(m_Param.apfQ[0]);
			ui->m_APF1QValue->setValue(m_Param.apfQ[1]);

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
	AS::CombParam& combParam = m_Param.comb.at(0);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime0->value(), ui->m_Feedback0->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb1() {
	AS::CombParam& combParam = m_Param.comb.at(1);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime1->value(), ui->m_Feedback1->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb2() {
	AS::CombParam& combParam = m_Param.comb.at(2);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime2->value(), ui->m_Feedback2->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_Comb3() {
	AS::CombParam& combParam = m_Param.comb.at(3);
	if (auto ui = m_wpUi.lock())
		combParam = AS::CombParam(ui->m_DelayTime3->value(), ui->m_Feedback3->value());
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_APF0() {
	auto& freq = m_Param.apfFreq.at(0);
	auto& QVal = m_Param.apfQ.at(0);
	if (auto ui = m_wpUi.lock()) {
		freq = ui->m_APF0FreqSlider->value();
		QVal = ui->m_APF0QValue->value();
		ui->m_APF0FreqValue->setText(QString::number(freq));
	}
	if (auto rev = m_wpEffect.lock())
		rev->SetEffectParam(m_Param);
}

void ASQt_ReverbTab::ChangeParam_APF1() {
	auto& freq = m_Param.apfFreq.at(1);
	auto& QVal = m_Param.apfQ.at(1);
	if (auto ui = m_wpUi.lock()) {
		freq = ui->m_APF1FreqSlider->value();
		ui->m_APF1FreqValue->setText(QString::number(freq));
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