#include "DeviceManager.h"

#pragma comment(lib, "d3d11.lib")
DeviceManager::DeviceManager()
{
	// create DXGIFactory, d3d device

	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	/*D3D_DRIVER_TYPE driverTypes[] =
	{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE,

	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );*/

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL	curLevel;
	//�����豸ID3D11Device���豸������ID3D11DeviceContext
	hr = D3D11CreateDevice(
		NULL, //��ΪNULL��ѡ��Ĭ�ϵ�������
		D3D_DRIVER_TYPE_HARDWARE,//DriverType�����������ͣ�һ���������ѡ��Ӳ������
		NULL,//FlagsΪ��ѡ������һ��ΪNULL ,D3D11_CREATE_DEVICE_DEBUG
		createDeviceFlags,
		featureLevels,//Ϊ�����ṩ������������ȼ���һ������
		numFeatureLevels,//������Ԫ�ظ���
		D3D11_SDK_VERSION,//SDKVersion�㶨ΪD3D11_SDK_VERSION
		&m_pd3dDevice,
		&curLevel,//pFeatureLevelΪ������ѡ�е������ȼ�
		&m_pImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Craete device failed!", L"ERROR", MB_OK);
		return ;
	}
	//������������Ҫ��ýӿ�IDXGIFactory�������ǹ̶���
	//ͨ������������ýӿ�IDXGIFactory�������������� 
	IDXGIDevice *pDxgiDevice(NULL);
	hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (FAILED(hr))
	{
		return ;
	}
	IDXGIAdapter *pDxgiAdapter(NULL);
	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));	
	SAFE_RELEASE(pDxgiAdapter);
	SAFE_RELEASE(pDxgiDevice);
	D3D_FEATURE_LEVEL featureLevel = m_pd3dDevice->GetFeatureLevel();


}


DeviceManager::~DeviceManager()
{
	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pDXGIFactory);
/*
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3dDebug;
	HRESULT hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	if (d3dDebug != nullptr)			d3dDebug->Release();
#endif*/
	SAFE_RELEASE(m_pd3dDevice);
}

DeviceManager* DeviceManager::GetInstancePtr()
{
	static DeviceManager _instance;
	return &_instance;
}

DeviceManager& DeviceManager::GetInstance()
{
	return *GetInstancePtr();
}

