#define OLC_PGE_APPLICATION
#include "Engine.h"
#include "Kernel.h"

float fWorldScale;
olc::vf2d vWorldOffset;
struct Node
{
	std::string name="";
	bool customName=false;
	olc::vf2d pos;
	olc::Pixel col = olc::DARK_GREEN;

	Node(olc::vf2d pos):pos(pos){}
	void WorldToScreen(const olc::vf2d& v, float& nScreenX, float& nScreenY)
	{
		nScreenX = (int)((v.x - vWorldOffset.x) * fWorldScale);
		nScreenY = (int)((v.y - vWorldOffset.y) * fWorldScale);
	}

	void Draw(olc::PixelGameEngine* pge)
	{
		int length = name.size();
		float fRadius = 2.5;
		float ex, ey,sx, sy;
		WorldToScreen(pos, sx, sy);
		pge->FillCircle(sx, sy, (int32_t)(fRadius * fWorldScale), col);\
		WorldToScreen({ pos.x - (float)length / 2,pos.y - 0.6f }, ex, ey);
		pge->DrawString(ex, ey, name, olc::WHITE, 0.2*fWorldScale);
	}

};

std::vector<std::string> tools={"Traffic","Move","Connect","Remove","Node","Algo"};


class TopGen : public olc::PixelGameEngine
{
public:
	TopGen()
	{
		sAppName = "TopGen";
	}

private:
	olc::vf2d vOffset = { 0.0f, 0.0f };
	olc::vf2d vStartPan = { 0.0f, 0.0f };
	float fScale = 10.0f;
	float fGrid = 1.0f;
	float sbsize = ScreenWidth() * 0.2;
	void WorldToScreen(const olc::vf2d& v, int& nScreenX, int& nScreenY)
	{
		nScreenX = (int)((v.x - vOffset.x) * fScale);
		nScreenY = (int)((v.y - vOffset.y) * fScale);
	}

	void ScreenToWorld(int nScreenX, int nScreenY, olc::vf2d& v)
	{
		v.x = (float)(nScreenX) / fScale + vOffset.x;
		v.y = (float)(nScreenY) / fScale + vOffset.y;
	}

	void sreset(std::ostringstream& o) {
		o.str("");
		o.clear();
	}

	int currTool=1;
	Node* temp = NULL;
	std::vector<Node*> vecNodes;

	std::vector<Node*>::iterator selectedNode=vecNodes.end();
	std::vector < std::pair<int,int>>vecConn,vecTraf;
	olc::vf2d vCursor = { 0, 0 };


public:
	bool OnUserCreate() override
	{
		vOffset = { (float)(-ScreenWidth() / 2) / fScale, (float)(-ScreenHeight() / 2) / fScale };
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		float sboffset = ScreenWidth() * 0.2;
		olc::vf2d vMouse = { (float)GetMouseX(), (float)GetMouseY() };


		if (vMouse.x > sboffset) {
			if (GetMouse(2).bPressed)
			{
				vStartPan = vMouse;
			}

			if (GetMouse(2).bHeld)
			{
				vOffset -= (vMouse - vStartPan) / fScale;
				vStartPan = vMouse;
			}

			olc::vf2d vMouseBeforeZoom;
			ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseBeforeZoom);

			if (GetKey(olc::Key::Q).bHeld || GetMouseWheel() > 0)
			{
				fScale *= 1.1f;
			}

			if (GetKey(olc::Key::A).bHeld || GetMouseWheel() < 0)
			{
				fScale *= 0.9f;
			}

			olc::vf2d vMouseAfterZoom;
			ScreenToWorld((int)vMouse.x, (int)vMouse.y, vMouseAfterZoom);
			vOffset += (vMouseBeforeZoom - vMouseAfterZoom);


			vCursor.x = floorf((vMouseAfterZoom.x + 0.5f) * fGrid);
			vCursor.y = floorf((vMouseAfterZoom.y + 0.5f) * fGrid);

		}
		else {
			if (GetMouseWheel() > 0 && currTool<tools.size())
			{
				currTool++;
			}

			if (GetMouseWheel() < 0 && currTool > 1)
			{
				currTool--;
			}
		}
		bool select=false;


		if (GetMouse(0).bReleased) {
			for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
			{
				if ((vCursor - vecNodes[it - vecNodes.begin()]->pos).mag() < 2.5f) {
					selectedNode = it;
					select = true;
					break;
				}

			}
			
		}



		bool olp=false;
		std::string currT = tools[currTool - 1];

		if (currT == "Node") {
			if (GetMouse(0).bReleased)
			{
				for (auto& n : vecNodes)
				{
					if ((vCursor - n->pos).mag() < 5.0f) {
						olp = true;
						break;
					}
						
				}
				if (!olp) {
					temp = new Node(vCursor);
					temp->name = std::to_string(vecNodes.size());
					vecNodes.push_back(temp);
				}
			}
		}
		else if (currT == "Remove") {
			if (GetMouse(1).bReleased && selectedNode >= vecNodes.begin() && selectedNode < vecNodes.end()&& vecNodes.size()>0)
			{
				int din = std::distance(vecNodes.begin(), selectedNode);
				vecNodes.erase(selectedNode);
				for (auto i =din;i< vecNodes.size();i++)
				{
					if(!vecNodes[i]->customName)
						vecNodes[i]->name = std::to_string(i);
				}
				for (auto i = 0; i < vecConn.size(); i++)
				{
					if (vecConn[i].first == din || vecConn[i].second == din) {
						vecConn.erase(vecConn.begin() + i);
						i--;
						continue;
					}
					if (vecConn[i].first > din) {
						vecConn[i].first--;
					}
					if (vecConn[i].second > din) {
						vecConn[i].second--;
					}
				}
				for (auto i = 0; i < vecTraf.size(); i++)
				{
					if (vecTraf[i].first == din || vecTraf[i].second == din) {
						vecTraf.erase(vecTraf.begin() + i);
						i--;
						continue;
					}
					if (vecTraf[i].first > din) {
						vecTraf[i].first--;
					}
					if (vecTraf[i].second > din) {
						vecTraf[i].second--;
					}
				}
				selectedNode = vecNodes.end();
			}

		}else if (currT == "Move") {
			if (GetMouse(1).bReleased)
			{
				vecNodes[selectedNode - vecNodes.begin()]->pos = vCursor;
			}

		}else if (currT == "Connect") {
			if (GetMouse(1).bReleased && selectedNode >= vecNodes.begin() && selectedNode < vecNodes.end() && vecNodes.size()>0)
			{
				for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
				{
					if ((vCursor - vecNodes[it - vecNodes.begin()]->pos).mag() < 2.5f && selectedNode != it) {
						bool clear = true;
						std::pair<int, int> o = { std::distance(vecNodes.begin(),selectedNode),std::distance(vecNodes.begin(),it) }, m = { o.second,o.first };
						for (auto& i : vecConn)
						{
							if (i == o || i == m) {
								clear = false;
								break;
							}
						}
						if (clear) vecConn.push_back(o);
						break;
					}

				}
			}

		}else if (currT == "Traffic") {
			if (GetMouse(1).bReleased && selectedNode >= vecNodes.begin() && selectedNode < vecNodes.end() && vecNodes.size()>0)
			{
				for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
				{
					if ((vCursor - vecNodes[it - vecNodes.begin()]->pos).mag() < 2.5f && selectedNode != it) {
						bool clear = true;
						std::pair<int, int> o = { std::distance(vecNodes.begin(),selectedNode),std::distance(vecNodes.begin(),it) }, m = { o.second,o.first };
						for (auto& i : vecTraf)
						{
							if (i == o || i == m) {
								clear = false;
								break;
							}
						}
						if (clear) vecTraf.push_back(o);
						break;
					}

				}
			}

		}



		if(select)
			for (auto it = vecNodes.begin(); it != vecNodes.end(); it++)
			{
				if (selectedNode == it)
					vecNodes[it - vecNodes.begin()]->col = olc::DARK_GREY;
				else
					vecNodes[it - vecNodes.begin()]->col = olc::DARK_GREEN;

			}

		
		Clear(olc::VERY_DARK_BLUE);

		int sx, sy;
		int ex, ey;

		olc::vf2d vWorldTopLeft, vWorldBottomRight;
		ScreenToWorld(sboffset, 0, vWorldTopLeft);
		ScreenToWorld(ScreenWidth(), ScreenHeight(), vWorldBottomRight);

		vWorldTopLeft.x = floor(vWorldTopLeft.x) +1;
		vWorldTopLeft.y = floor(vWorldTopLeft.y);
		vWorldBottomRight.x = ceil(vWorldBottomRight.x);
		vWorldBottomRight.y = ceil(vWorldBottomRight.y);

		for (auto& i : vecTraf)
		{
			WorldToScreen(vecNodes[i.first]->pos, sx, sy);
			WorldToScreen(vecNodes[i.second]->pos, ex, ey);
			DrawLine(sx, sy, ex, ey,olc::GREEN,100);
		}
		for (float x = vWorldTopLeft.x; x < vWorldBottomRight.x; x += fGrid)
		{
			for (float y = vWorldTopLeft.y; y < vWorldBottomRight.y; y += fGrid)
			{
				WorldToScreen({ x, y }, sx, sy);
				Draw(sx, sy, olc::BLUE);
			}
		}

		WorldToScreen({ 0,vWorldTopLeft.y }, sx, sy);
		WorldToScreen({ 0,vWorldBottomRight.y }, ex, ey);
		DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);
		WorldToScreen({ vWorldTopLeft.x,0 }, sx, sy);
		WorldToScreen({ vWorldBottomRight.x,0 }, ex, ey);
		DrawLine(sx, sy, ex, ey, olc::GREY, 0xF0F0F0F0);

		fWorldScale = fScale;
		vWorldOffset = vOffset;
		for(auto& i : vecConn)
		{
			 WorldToScreen(vecNodes[i.first]->pos, sx, sy);
			 WorldToScreen(vecNodes[i.second]->pos, ex, ey);
			 DrawLine(sx,sy,ex,ey);
		}
		
		for (auto& shape : vecNodes)
		{
			shape->Draw(this);
		}
		WorldToScreen(vCursor, sx, sy);
		DrawCircle(sx, sy, 3, olc::YELLOW);

		//DrawString(10, 10, "X=" + std::to_string(vCursor.x) + ", Y=" + std::to_string(vCursor.y)+"  "+std::to_string(sbsize), olc::YELLOW, 2);
		
		int count = tools.size();
		int tsx = ScreenWidth() * 0.05,tex= ScreenWidth() * 0.1;
		float elH=ScreenHeight() * 0.7 / count;
		float i = ScreenHeight() * 0.15;
		while (count) {
			if (currTool == count)
				FillRect(tsx, i, tex, elH,olc::GREEN);
			else
				FillRect(tsx, i, tex, elH);
			DrawString(((tsx+tex)/2), i + elH / 2,tools[count - 1],olc::BLUE,1);
			i += elH;
			count--;
		}


		DrawRect(0, 0, ScreenWidth() * 0.2, ScreenHeight(),olc::BLUE);
		return true;
	}

	void Export() {
		std::ostringstream oss,ip;
		ExportFile t("test1");
		t.CopyToOut("HEADERS");
		t.CopyToOut("OPEN");
		
		t.FormatToOut("NODECREATE", { std::to_string(vecNodes.size()) });
		//connection
		 for (int i = 0; i < vecConn.size(); i++){
			t.FormatToOut("NODECONTAINERS", { std::to_string(vecConn[i].first),std::to_string(vecConn[i].second),std::to_string(vecConn[i].first),std::to_string(vecConn[i].second),std::to_string(vecConn[i].first),std::to_string(vecConn[i].second),std::to_string(vecConn[i].first),std::to_string(vecConn[i].second) });
		}
		t.FormatToOut("INSTALLATTRIBUTES", { std::to_string(vecConn[0].first),std::to_string(vecConn[0].second) });
		//ip
		for (int i = 0; i < vecConn.size(); i++){
			t.FormatToOut("IPASSIGNMENT", { std::to_string((i / 256)+1),std::to_string((i % 256)+1),std::to_string(vecConn[i].first),std::to_string(vecConn[i].second),std::to_string(vecConn[i].first),std::to_string(vecConn[i].second) });
		}
		//trafiic
		for (int i = 0; i < vecTraf.size(); i++){
			std::string a="Address not found", b=a, c=a;
			for (auto& it : vecConn){
				if (it.first == vecTraf[i].second||it.second == vecTraf[i].second) {
					a = std::to_string(it.first);
					b = std::to_string(it.second);
					it.first == vecTraf[i].second?c="0":c="1";
					break;
				}
			}
			t.FormatToOut("TRAFFICGEN", {	std::to_string(vecTraf[i].second),std::to_string(8080+i),
											std::to_string(vecTraf[i].second),a,b,c, std::to_string(vecTraf[i].second),
											std::to_string(vecTraf[i].second), std::to_string(vecTraf[i].second), 
											std::to_string(vecTraf[i].second),std::to_string(vecTraf[i].second),std::to_string(vecTraf[i].second),
											std::to_string(vecTraf[i].second),
											std::to_string(vecTraf[i].second),
											std::to_string(vecTraf[i].first), std::to_string(vecTraf[i].first), 
											std::to_string(i),
											std::to_string(i), std::to_string(vecTraf[i].first), std::to_string(vecTraf[i].second),
											std::to_string(vecTraf[i].first),std::to_string(i),
											std::to_string(i),
											std::to_string(i)});
		}
		t.FormatToOut("OUTPUTSETTINGS", {});
		t.FormatToOut("SIMULATE", {"200.0"});
		t.CopyToOut("CLOSE");
	}
};


int main()
{
	TopGen demo;
	if (demo.Construct(1600, 960, 1, 1)) {
		demo.Start();
		demo.Export();
	}
	
	return 0;
}



