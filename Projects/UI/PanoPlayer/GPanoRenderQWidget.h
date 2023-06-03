/*
 * Copyright (c) 2015-2023 Pengju Lu, Yanli Wang

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
 
 
#ifndef G_PANO_RENDER_QWIDGET_H_
#define G_PANO_RENDER_QWIDGET_H_

#include <thread>
#include <map>

#include <QApplication>
#include <QMouseEvent>
#include <QWidget>


class GPanoRenderQWidget : public QWidget
{
	Q_OBJECT

public:
	GPanoRenderQWidget(QWidget *parent = 0);

	virtual QPaintEngine* paintEngine() const;

	void mouseMoveEvent(QMouseEvent *event);

	void mousePressEvent(QMouseEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void wheelEvent(QWheelEvent *event);

	void resizeEvent(QResizeEvent *event);

	void SetProfilePath(const QString& sPath);

	bool OpenVideos(const std::vector<QString>& vVideoPathes);

	std::vector<QString> GetPanoTypeList();

	bool SetPanoType(const QString& sPanoType);

	bool SnapShot();

	void Release();

private:
	static void FeedIn(void* GPanoRenderQWidgetObj, int iCameraIdx);

	void SetupPixelFormat(void* hWnd);

	void CreatePanoRender();

private:
	std::map<int, std::string> m_mpPanoType2String;
	std::map<std::string, int> m_mpString2PanoType;

	std::vector<std::string> m_vVideoPathes;

	std::string m_sProfilePath;
	std::vector<std::thread> m_vtdFeedIn;
	bool m_bFeedingFlag;

	int m_iVideoSourceHandle;
	int m_iPanoRenderHandle;

	bool m_bMouseLeftDown;
	int m_iX0;
	int m_iY0;
	int m_iWidth;
	int m_iHeight;

	std::vector<int> m_vSupportedPanoTypeList;
};

#endif //G_PANO_RENDER_QWIDGET_H_
