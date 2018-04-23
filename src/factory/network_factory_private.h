/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for 
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** This code based on Trolltechs QSA input dialog factory.
**
** Copyright (C) 2007  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************/

#include <QObject>
#include <QtNetwork>

namespace QS{

class UdpServer : public QObject{
Q_OBJECT
public:
	UdpServer(const QString &port){
		qDebug() << port;
		s = new QUdpSocket(this);
		s->bind(port.toInt());
		connect(s, SIGNAL(readyRead()), this, SLOT(readyRead()));
		}

	~UdpServer(){}

public slots:
	void readyRead(){
		while (s->hasPendingDatagrams()) {
			QByteArray datagram;
			datagram.resize(s->pendingDatagramSize());
			QHostAddress sender;
			quint16 senderPort;
			s->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
			emit packed(QString( datagram));
     			}
		}


signals:
	void packed(QString);
private:
	QUdpSocket *s;
	};


class TcpClient : public QObject{
Q_OBJECT
public:
	TcpClient(const QString& hostname){
		QStringList l = hostname.split(":");
		tcpSocket = new QTcpSocket(this);
		tcpSocket->connectToHost(l[0],l[1].toInt());
		//qDebug() << l[0] << l[1].toInt();
		connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
		}

	~TcpClient(){
		delete tcpSocket;
		}
public slots:
	void readyRead(){
		int b = tcpSocket->bytesAvailable();
		do{
			bytes = b;
			emit ready();
			b = tcpSocket->bytesAvailable();
			}while(b != bytes && b!=0);
		}

	QString readLine(){
		return QString(tcpSocket->readLine(1000));
		}

	void sendLine(const QString& l){
		tcpSocket->write(QByteArray(l.toLatin1()));
		//qDebug() << "<<<" <<  QByteArray(l.toLatin1());
		}

signals:
	void ready();
private:
	QTcpSocket *tcpSocket;
	int bytes;

	};


} // namespace QS
