/*
 Navicat Premium Data Transfer

 Source Server         : weisang
 Source Server Type    : MySQL
 Source Server Version : 80036
 Source Host           : localhost:3306
 Source Schema         : user_privilege_manager_system

 Target Server Type    : MySQL
 Target Server Version : 80036
 File Encoding         : 65001

 Date: 05/05/2024 20:53:50
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for privilege
-- ----------------------------
DROP TABLE IF EXISTS `privilege`;
CREATE TABLE `privilege`  (
  `number` int NOT NULL AUTO_INCREMENT COMMENT '权限编号',
  `privilege` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '权限名称',
  `descr` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '权限描述',
  `area` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '适用范围',
  PRIMARY KEY (`number`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of privilege
-- ----------------------------
INSERT INTO `privilege` VALUES (1, 'read_access', '对用户的读权限', '通常适用于普通用户或只需查看信息的角色。');
INSERT INTO `privilege` VALUES (2, 'edit_access', '对用户的修改(他人)权限', '通常适用于具有编辑和更新权限的管理角色。');
INSERT INTO `privilege` VALUES (3, 'add_access', '对用户的添加权限', '通常适用于一般管理员。');
INSERT INTO `privilege` VALUES (4, 'delete_access', '删除、禁用、启用用户权限', '通常只赋予少数高级管理角色,以防止误操作。');

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `id` bigint NOT NULL AUTO_INCREMENT COMMENT 'ID',
  `user_id` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '用户ID',
  `username` varchar(20) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '用户名',
  `gender` tinyint NOT NULL DEFAULT 0 COMMENT '性别 0=未知，1=男，2=女',
  `mobile` varchar(18) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '电话号码',
  `email` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '电子邮箱',
  `password` varchar(18) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '密码',
  `avatar_path` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '头像路径',
  `isEnable` tinyint NULL DEFAULT 1 COMMENT '用户是否启用',
  `isDeleted` tinyint NULL DEFAULT 0 COMMENT '用户是否被删除',
  `created` datetime NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  PRIMARY KEY (`id`) USING BTREE,
  UNIQUE INDEX `user_id`(`user_id` ASC) USING BTREE,
  UNIQUE INDEX `mobile`(`mobile` ASC) USING BTREE,
  UNIQUE INDEX `email`(`email` ASC) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 11232 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES (1, '12345678', '威桑', 1, '0203-1594', '3658269698@qq.com', '741852', 'images/avatar/12345678.jpg', 1, 0, '2024-03-30 09:53:52');
INSERT INTO `user` VALUES (5, '11111111', 'Jack', 1, '1730796969', 'dogepi@QQ.com', '963852', NULL, 1, 0, '2024-04-03 09:03:23');
INSERT INTO `user` VALUES (11094, '22222222', 'zasx', 2, '54354634', '336536534', '10101010', NULL, 0, 0, '2024-05-02 22:05:24');
INSERT INTO `user` VALUES (11227, '20240505202259', 'asdad', 1, '25452432', '453245354', '123456', NULL, 1, 0, '2024-05-05 20:23:08');

-- ----------------------------
-- Table structure for user_privilege
-- ----------------------------
DROP TABLE IF EXISTS `user_privilege`;
CREATE TABLE `user_privilege`  (
  `user_id` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `username` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL,
  `privilege_read` int NOT NULL DEFAULT 1,
  `privilege_edit` int NOT NULL DEFAULT 0,
  `privilege_add` int NOT NULL DEFAULT 0,
  `privilege_delete` int NOT NULL DEFAULT 0,
  `isDeleted` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`user_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of user_privilege
-- ----------------------------
INSERT INTO `user_privilege` VALUES ('11111111', 'Jack', 1, 1, 0, 0, 0);
INSERT INTO `user_privilege` VALUES ('12345678', '威桑', 1, 1, 1, 1, 0);
INSERT INTO `user_privilege` VALUES ('22222222', 'zasx', 1, 0, 0, 0, 0);

SET FOREIGN_KEY_CHECKS = 1;
