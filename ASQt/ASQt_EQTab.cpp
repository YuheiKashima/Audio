#include "ASQt_EQTab.h"

ASQt_EQTab::ASQt_EQTab(std::weak_ptr<Ui::ASQt_SourceClass> _parent, std::weak_ptr<AS::Equalizer> _effect) :m_wpUi(_parent), m_wpEffect(_effect) {
	if (auto ui = m_wpUi.lock()) {
		connect(ui->m_55Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_77Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_110Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_156Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_220Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_311Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_440Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_622Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_880Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_1200Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_1800Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_2500Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_3500Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_5000Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_7000Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_10000Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_14000Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_20000Hz, &QSlider::sliderReleased, this, &ASQt_EQTab::ChangeParam);
		connect(ui->m_EnableEq, &QCheckBox::stateChanged, this, &ASQt_EQTab::ChangeEnable);
	}
}

ASQt_EQTab::~ASQt_EQTab() {
}

void ASQt_EQTab::ChangeParam() {
	auto ui = m_wpUi.lock();
	if (auto eq = m_wpEffect.lock()) {
		m_Param = AS::EQParam(
			1.0f,
			ui->m_55Hz->value(),
			ui->m_77Hz->value(),
			ui->m_110Hz->value(),
			ui->m_156Hz->value(),
			ui->m_220Hz->value(),
			ui->m_311Hz->value(),
			ui->m_440Hz->value(),
			ui->m_622Hz->value(),
			ui->m_880Hz->value(),
			ui->m_1200Hz->value(),
			ui->m_1800Hz->value(),
			ui->m_2500Hz->value(),
			ui->m_3500Hz->value(),
			ui->m_5000Hz->value(),
			ui->m_7000Hz->value(),
			ui->m_10000Hz->value(),
			ui->m_14000Hz->value(),
			ui->m_20000Hz->value(),
			1.0f
		);
		eq->SetEffectParam(m_Param);
	}
}

void ASQt_EQTab::ChangeEnable() {
	auto ui = m_wpUi.lock();
	if (auto eq = m_wpEffect.lock()) {
		eq->SetEnable(ui->m_EnableEq->checkState() == Qt::CheckState::Checked);
	}
}