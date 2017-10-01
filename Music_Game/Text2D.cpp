#include "Tex2Dt.h"

//---------------------------------------------------------
//Default Constructor using Arial as the default font
//---------------------------------------------------------
Text2D::Text2D(unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;
	initWidth = _width;
	initHeight = _height;

	textList = std::vector<textObject>();
	path = L"Assets/fonts/Arial.spritefont";
}

//---------------------------------------------------------
//Constructor taking a font file path
//---------------------------------------------------------
Text2D::Text2D(const wchar_t* _path, unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;
	initWidth = _width;
	initHeight = _height;

	textList = std::vector<textObject>();
	path = _path;
}

//---------------------------------------------------------
//Default Deconstructor
//---------------------------------------------------------
Text2D::~Text2D()
{
	//fontSRV->Release();
	delete spriteBatch;
	delete spriteFontArial;
}

//---------------------------------------------------------
//Inititialize members
//---------------------------------------------------------
void Text2D::Init(ID3D11DeviceContext* context, ID3D11Device* device)
{
	// Set up 2D rendering stuff
	spriteBatch = new DirectX::SpriteBatch(context);
	spriteFontArial = new DirectX::SpriteFont(device, path);
}

void Text2D::SetText(const wchar_t* _text)
{
	textToDraw = _text;
}


//---------------------------------------------------------
//Setup the font
//---------------------------------------------------------
void Text2D::SetupFont()
{
	spriteFontArial->GetSpriteSheet(&fontSRV);
}

//---------------------------------------------------------
//Draw the list of text objects
//---------------------------------------------------------
void Text2D::DrawMyText()
{
	//Begin the sprite batch drawing
	spriteBatch->Begin();

	//loop through the different items to draw
	if (textList.size() > 0)
	{
		for (unsigned int t = 0; t < textList.size(); t++)
		{
			spriteFontArial->DrawString(
				spriteBatch,
				textList.at(t).text,
				textList.at(t).position);
		}
	}
	//end drawing
	spriteBatch->End();
}

//---------------------------------------------------------
//Draw the list of text objects
//---------------------------------------------------------
void Text2D::DrawLiveText(const wchar_t* text, DirectX::XMFLOAT2 pos)
{
	//Begin the sprite batch drawing
	spriteBatch->Begin();

	spriteFontArial->DrawString(
		spriteBatch,
		text,
		pos);

	//end drawing
	spriteBatch->End();
}

void Text2D::DrawLiveText(int score, DirectX::XMFLOAT2 pos)
{
	std::wstring s = std::to_wstring(score);
	wchar_t* wc = const_cast<wchar_t*>(s.c_str());

	//Use the default screen size to calculate where to text should display
	float xPercent = (pos.x * 100.0f) / (float)initWidth;
	float yPercent = (pos.y * 100.0f) / (float)initHeight;

	DirectX::XMFLOAT2 position;

	position.x = ((float)width * xPercent) / 100.0f;
	position.y = ((float)height * yPercent) / 100.0f;

	//Begin the sprite batch drawing
	spriteBatch->Begin();

	spriteFontArial->DrawString(
		spriteBatch,
		wc,
		position);

	//end drawing
	spriteBatch->End();
}

//---------------------------------------------------------
//Add a text object to be drawn
//---------------------------------------------------------
void Text2D::AddText(const wchar_t* _text, DirectX::XMFLOAT2 _pos)
{
	textObject temp;
	temp.text = _text;
	temp.position = _pos;

	//Find the percentage location
	temp.xPercent = (_pos.x * 100.0f) / (float)width;
	temp.yPercent = (_pos.y * 100.0f) / (float)height;

	textList.push_back(temp);
}

//---------------------------------------------------------
//Clear the text list
//---------------------------------------------------------
void Text2D::ClearText()
{
	if (textList.size() > 0)
	{
		textList.clear();
	}
}

//---------------------------------------------------------
//Return the font's SRV
//---------------------------------------------------------
ID3D11ShaderResourceView* Text2D::GetSRV()
{
	return fontSRV;
}

//---------------------------------------------------------
//Adjust the text position if the screen size has changed
//form the defualt size
//---------------------------------------------------------
void Text2D::AdjustPosition(unsigned int _width, unsigned int _height)
{
	width = _width;
	height = _height;

	if (textList.size() > 0)
	{
		for (unsigned int i = 0; i < textList.size(); i++)
		{
			textList.at(i).position.x = ((float)width * textList.at(i).xPercent) / 100.0f;
			textList.at(i).position.y = ((float)height * textList.at(i).yPercent) / 100.0f;
		}
	}
}