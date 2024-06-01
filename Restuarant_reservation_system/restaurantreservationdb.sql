-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Mar 14, 2024 at 02:50 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `restaurantreservationdb`
--

-- --------------------------------------------------------

--
-- Table structure for table `cart`
--

CREATE TABLE `cart` (
  `cart_id` int(11) NOT NULL,
  `username` varchar(50) NOT NULL,
  `food_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL,
  `status` varchar(30) NOT NULL,
  `reservation_id` int(11) DEFAULT NULL,
  `added_at` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `cart`
--

INSERT INTO `cart` (`cart_id`, `username`, `food_id`, `quantity`, `status`, `reservation_id`, `added_at`) VALUES
(1, 'jane43', 3, 1, 'DEPOSIT PAID', 0, '2024-03-08 02:25:36'),
(2, 'jane43', 2, 1, 'DEPOSIT PAID', 7, '2024-03-08 18:29:24'),
(3, 'jane43', 2, 1, 'DEPOSIT PAID', 9, '2024-03-08 18:55:32'),
(4, 'jane43', 1, 1, 'DEPOSIT PAID', 9, '2024-03-09 20:42:15'),
(6, 'jane43', 2, 1, 'DEPOSIT PAID', 9, '2024-03-13 20:24:15'),
(7, 'jane43', 3, 1, 'DEPOSIT PAID', 10, '2024-03-13 22:23:44'),
(8, 'jane43', 2, 1, 'DEPOSIT PAID', 10, '2024-03-14 16:40:02');

-- --------------------------------------------------------

--
-- Table structure for table `foods`
--

CREATE TABLE `foods` (
  `food_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) NOT NULL,
  `price` decimal(9,2) NOT NULL,
  `image_path` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `foods`
--

INSERT INTO `foods` (`food_id`, `name`, `description`, `price`, `image_path`) VALUES
(1, 'muffins', 'Tasty muffins', 200.00, 'IMG/muffins.jpg'),
(2, 'Chicken', 'Deep Fried Chicken', 500.00, 'IMG/chicken.jpg'),
(3, 'Noodles', 'Noodle soup', 200.00, 'IMG/noodles.jpg');

-- --------------------------------------------------------

--
-- Table structure for table `reservations`
--

CREATE TABLE `reservations` (
  `reservation_id` int(11) NOT NULL,
  `reservation_date` date NOT NULL,
  `reservation_time` time NOT NULL,
  `duration` int(11) NOT NULL,
  `status` varchar(255) NOT NULL,
  `username` varchar(25) NOT NULL,
  `table_id` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `reservations`
--

INSERT INTO `reservations` (`reservation_id`, `reservation_date`, `reservation_time`, `duration`, `status`, `username`, `table_id`) VALUES
(1, '2024-03-08', '02:35:00', 1, 'Pending', 'jane43', 1),
(5, '2024-03-08', '05:04:00', 1, 'APPROVED', 'jane43', 1),
(6, '2024-03-08', '11:04:00', 2, 'Pending', 'jane43', 1),
(7, '2024-03-09', '03:09:00', 1, 'CANCELLED', 'jane43', 1),
(8, '2024-03-14', '22:43:00', 1, 'Pending', 'jane43', 1),
(9, '2024-03-15', '16:44:00', 1, 'Pending', 'jane43', 1),
(10, '2024-03-21', '18:46:00', 2, 'Pending', 'jane43', 1);

-- --------------------------------------------------------

--
-- Table structure for table `tables`
--

CREATE TABLE `tables` (
  `table_id` int(11) NOT NULL,
  `capacity` int(11) NOT NULL,
  `status` varchar(25) NOT NULL,
  `table_name` varchar(25) NOT NULL,
  `location` varchar(25) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `tables`
--

INSERT INTO `tables` (`table_id`, `capacity`, `status`, `table_name`, `location`) VALUES
(1, 4, 'Available', 'Simba', 'Table 1'),
(3, 5, 'Available', 'Ndovu', 'Table 2');

-- --------------------------------------------------------

--
-- Table structure for table `transactions`
--

CREATE TABLE `transactions` (
  `id` int(11) NOT NULL,
  `item_number` varchar(50) DEFAULT NULL,
  `item_name` varchar(255) DEFAULT NULL,
  `item_price` float(10,2) DEFAULT NULL,
  `item_price_currency` varchar(10) DEFAULT NULL,
  `payer_id` varchar(50) DEFAULT NULL,
  `payer_name` varchar(50) DEFAULT NULL,
  `payer_email` varchar(50) DEFAULT NULL,
  `payer_country` varchar(20) DEFAULT NULL,
  `merchant_id` varchar(255) DEFAULT NULL,
  `merchant_email` varchar(50) DEFAULT NULL,
  `order_id` varchar(50) NOT NULL,
  `transaction_id` varchar(50) NOT NULL,
  `paid_amount` float(10,2) NOT NULL,
  `paid_amount_currency` varchar(10) NOT NULL,
  `payment_source` varchar(50) DEFAULT NULL,
  `payment_status` varchar(25) NOT NULL,
  `created` datetime NOT NULL,
  `modified` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `transactions`
--

INSERT INTO `transactions` (`id`, `item_number`, `item_name`, `item_price`, `item_price_currency`, `payer_id`, `payer_name`, `payer_email`, `payer_country`, `merchant_id`, `merchant_email`, `order_id`, `transaction_id`, `paid_amount`, `paid_amount_currency`, `payment_source`, `payment_status`, `created`, `modified`) VALUES
(1, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '21E07372L5876294Y', '2VJ57513VN6483418', 250.00, 'USD', 'paypal', 'COMPLETED', '2024-03-09 19:04:45', '0000-00-00 00:00:00'),
(2, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '6TG30545RS889250T', '4P840515HM884872M', 2.50, 'USD', 'paypal', 'COMPLETED', '2024-03-09 20:40:45', '0000-00-00 00:00:00'),
(3, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '18S001766C935072J', '2HC84024H6102593Y', 1.00, 'USD', 'paypal', 'COMPLETED', '2024-03-09 20:44:03', '0000-00-00 00:00:00'),
(4, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '4J318475KB627881X', '993917912Y606261D', 2.50, 'USD', 'paypal', 'COMPLETED', '2024-03-13 22:18:37', '0000-00-00 00:00:00'),
(5, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '7LJ346381K033172P', '5XW40141KF4611426', 1.00, 'USD', 'paypal', 'COMPLETED', '2024-03-13 22:28:54', '0000-00-00 00:00:00'),
(6, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '2CH74582PJ287661F', '2KU331935B566883L', 1.00, 'USD', 'paypal', 'COMPLETED', '2024-03-13 22:29:16', '0000-00-00 00:00:00'),
(7, 'DP12345', 'Demo Product', 75.00, 'USD', 'BGTZHC7UUBFDN', 'John Doe', 'sb-sm3kc29863410@personal.example.com', 'KE', 'UM7M4RM5DRGPS', 'sb-dypte29846718@business.example.com', '0BW79323MS3407501', '4EJ82877UL729352G', 2.50, 'USD', 'paypal', 'COMPLETED', '2024-03-14 16:44:25', '0000-00-00 00:00:00');

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

CREATE TABLE `users` (
  `username` varchar(50) NOT NULL,
  `firstname` varchar(50) DEFAULT NULL,
  `lastname` varchar(50) DEFAULT NULL,
  `email` varchar(100) DEFAULT NULL,
  `phonenumber` varchar(15) DEFAULT NULL,
  `password` varchar(255) DEFAULT NULL,
  `role` varchar(20) DEFAULT NULL,
  `security_question` varchar(255) DEFAULT NULL,
  `security_answer` varchar(255) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `users`
--

INSERT INTO `users` (`username`, `firstname`, `lastname`, `email`, `phonenumber`, `password`, `role`, `security_question`, `security_answer`) VALUES
('james32', 'dan', 'doe', 'james@gmail.com', '0728819121', '25d55ad283aa400af464c76d713c07ad', 'customer', 'What is your mother\'s maiden name?', 'chicken'),
('jane43', 'jane', 'doe', 'janedoe@gmail.com', '0741525241', 'e99a18c428cb38d5f260853678922e03', 'customer', 'What is your favorite movie?', 'avengers'),
('john123', 'john', 'doe', 'johndoe@gmail.com', '0741525241', 'e99a18c428cb38d5f260853678922e03', 'admin', 'What is the name of your first pet?', 'bunny');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `cart`
--
ALTER TABLE `cart`
  ADD PRIMARY KEY (`cart_id`),
  ADD KEY `username` (`username`),
  ADD KEY `food_id` (`food_id`),
  ADD KEY `reservation_id` (`reservation_id`);

--
-- Indexes for table `foods`
--
ALTER TABLE `foods`
  ADD PRIMARY KEY (`food_id`);

--
-- Indexes for table `reservations`
--
ALTER TABLE `reservations`
  ADD PRIMARY KEY (`reservation_id`),
  ADD KEY `username` (`username`),
  ADD KEY `table_id` (`table_id`);

--
-- Indexes for table `tables`
--
ALTER TABLE `tables`
  ADD PRIMARY KEY (`table_id`);

--
-- Indexes for table `transactions`
--
ALTER TABLE `transactions`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`username`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `cart`
--
ALTER TABLE `cart`
  MODIFY `cart_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=9;

--
-- AUTO_INCREMENT for table `reservations`
--
ALTER TABLE `reservations`
  MODIFY `reservation_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=11;

--
-- AUTO_INCREMENT for table `tables`
--
ALTER TABLE `tables`
  MODIFY `table_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT for table `transactions`
--
ALTER TABLE `transactions`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `cart`
--
ALTER TABLE `cart`
  ADD CONSTRAINT `cart_ibfk_1` FOREIGN KEY (`username`) REFERENCES `users` (`username`) ON DELETE CASCADE,
  ADD CONSTRAINT `cart_ibfk_2` FOREIGN KEY (`food_id`) REFERENCES `foods` (`food_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `cart_ibfk_3` FOREIGN KEY (`reservation_id`) REFERENCES `reservations` (`reservation_id`) ON DELETE CASCADE;

--
-- Constraints for table `reservations`
--
ALTER TABLE `reservations`
  ADD CONSTRAINT `reservations_ibfk_1` FOREIGN KEY (`username`) REFERENCES `users` (`username`) ON DELETE CASCADE,
  ADD CONSTRAINT `reservations_ibfk_2` FOREIGN KEY (`table_id`) REFERENCES `tables` (`table_id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
