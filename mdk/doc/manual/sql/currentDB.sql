-- MySQL dump 9.10
--
-- Host: localhost    Database: m23
-- ------------------------------------------------------
-- Server version	4.0.18-log

--
-- Table structure for table `sourceslist`
--

DROP TABLE IF EXISTS sourceslist;
CREATE TABLE sourceslist (
  id int(11) NOT NULL auto_increment,
  name varchar(255) NOT NULL default '',
  distr varchar(255) NOT NULL default '',
  description mediumtext NOT NULL,
  list longtext NOT NULL,
  KEY id (id)
) TYPE=MyISAM;

--
-- Dumping data for table `sourceslist`
--


/*!40000 ALTER TABLE sourceslist DISABLE KEYS */;
LOCK TABLES sourceslist WRITE;
INSERT INTO sourceslist VALUES (4,'unstable','debian','for Debian/Sid','#unstable sources\r\ndeb-src http://ftp.de.debian.org/debian/ unstable main non-free contrib\r\ndeb http://non-us.debian.org/debian-non-US unstable/non-US main contrib non-free\r\ndeb-src http://non-us.debian.org/debian-non-US unstable/non-US main contrib non-free\r\ndeb http://ftp.de.debian.org/debian/ unstable main non-free contrib\r\n\r\n#knoppix packages: hwsetup, hwdata-knoppix, ddcxinfo-knoppix, xf86config-knoppix\r\n\r\ndeb http://m23.sourceforge.net/m23debs/ ./'),(62,'stable','debian','for Debian/Woody','#stable sources (woody)\r\ndeb http://non-us.debian.org/debian-non-US stable/non-US main contrib non-free\r\ndeb http://ftp.de.debian.org/debian/ stable main non-free contrib\r\n\r\n#knoppix packages: hwsetup, hwdata-knoppix, ddcxinfo-knoppix, xf86config-knoppix\r\n\r\ndeb http://m23.sourceforge.net/m23debs/ ./\r\n\r\n#for KDE 3.2\r\ndeb http://download.kde.org/stable/3.2/Debian/ stable main');
UNLOCK TABLES;
/*!40000 ALTER TABLE sourceslist ENABLE KEYS */;

