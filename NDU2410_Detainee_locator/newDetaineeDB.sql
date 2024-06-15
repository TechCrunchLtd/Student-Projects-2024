-- MySQL dump 10.13  Distrib 8.0.36, for Win64 (x86_64)
--
-- Host: localhost    Database: detainee_db
-- ------------------------------------------------------
-- Server version	8.0.35

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `adminusers`
--

DROP TABLE IF EXISTS `adminusers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `adminusers` (
  `email` varchar(50) NOT NULL,
  `names` varchar(45) NOT NULL,
  `password` varchar(45) NOT NULL,
  PRIMARY KEY (`email`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `adminusers`
--

LOCK TABLES `adminusers` WRITE;
/*!40000 ALTER TABLE `adminusers` DISABLE KEYS */;
INSERT INTO `adminusers` VALUES ('nadyah@admin.com','Lydia Nakamya','nadyah123'),('tracy@admin.com','Namutebi Beatrice','tracy123');
/*!40000 ALTER TABLE `adminusers` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `detaineecenters`
--

DROP TABLE IF EXISTS `detaineecenters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `detaineecenters` (
  `centerId` int NOT NULL AUTO_INCREMENT,
  `centerName` varchar(45) NOT NULL,
  `location` varchar(45) NOT NULL,
  `mappin` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`centerId`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `detaineecenters`
--

LOCK TABLES `detaineecenters` WRITE;
/*!40000 ALTER TABLE `detaineecenters` DISABLE KEYS */;
INSERT INTO `detaineecenters` VALUES (1,'Kampala central police station','City center','https://maps.app.goo.gl/vJMKqUiEaWH6gLoh8'),(2,'Kasangati police station','kasangati - gayaza road','https://maps.app.goo.gl/jmMQm23zbyuDSMq37'),(3,'Tororo Central Police station','Tororo town - Malaba road','https://maps.app.goo.gl/LmydUiNt9CMnCUnD7'),(4,'Central Police Station','Mbarara City','https://maps.app.goo.gl/uXTT3Woadon4SzdM9'),(5,'Central Police Station','Mbarara City','https://maps.app.goo.gl/uXTT3Woadon4SzdM9'),(6,'Kansangati police station','Mbarara City','https://maps.app.goo.gl/uXTT3Woadon4SzdM9'),(7,'Central Police Station','Masaka','https://maps.app.goo.gl/uXTT3Woadon4SzdM9'),(8,'Jinja Road Police Station','Kampala','https://maps.app.goo.gl/bkDcG2CxATeAWxTd7');
/*!40000 ALTER TABLE `detaineecenters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `detainees`
--

DROP TABLE IF EXISTS `detainees`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `detainees` (
  `NIN` varchar(20) NOT NULL,
  `fullNames` varchar(45) NOT NULL,
  `age` int NOT NULL,
  `center` int NOT NULL,
  `charges` varchar(200) NOT NULL,
  `arrestDate` datetime NOT NULL,
  `arrestArea` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`NIN`),
  KEY `centerName_idx` (`center`),
  CONSTRAINT `centers` FOREIGN KEY (`center`) REFERENCES `detaineecenters` (`centerId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `detainees`
--

LOCK TABLES `detainees` WRITE;
/*!40000 ALTER TABLE `detainees` DISABLE KEYS */;
INSERT INTO `detainees` VALUES ('CF23FFF7JK4P','Muganda Afrobis',55,2,'Attempted murder','2024-04-19 00:00:00','Bugolobi'),('CF4590FH457S','Baby Gal',22,4,'Thefty','2024-02-19 00:00:00',NULL),('CF75890116QSZU','Mary Babra',47,2,'Sexual Assults','2023-11-03 00:00:00',NULL),('CF81456899GPAA','Mary Maraim',26,1,'Domestic voilence - childe abuse','2023-12-25 00:00:00',NULL),('CM19024HG7PX','Ping Dee',29,5,'Impersonation','2024-04-13 00:00:00',NULL),('CM19024HG7PYY','Muganda Afrobis',51,1,'Attempted murder','2024-04-18 07:49:00','Bugolobi'),('CM55290148S3IP','Nakawuka Brenda Erone',29,8,'Uttering False Accusations and Cyber Harassment','2024-04-20 10:00:00','Forest Mall'),('CM55290148S3IU','Musa Moses',20,1,'Trison','2024-02-01 00:00:00',NULL),('CM6503012HOJL','Edwin Ssekamanje',24,3,'Domestic voilence','2024-03-29 00:00:00',NULL),('CM6P34595HH1','Peter King',33,2,'armed robbery','2024-04-01 00:00:00',NULL),('CM94060102FDKL','Konta Kinte',32,3,'Public nuisance','2024-04-11 00:00:00',NULL);
/*!40000 ALTER TABLE `detainees` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-04-19 20:19:50
