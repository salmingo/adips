/**
 * @class ADIWorkFlow 天文数字图像作业流程
 * @version 0.1
 * @date 2021-04
 * @note
 * 作业流程:
 * - 维护数据处理队列
 * - 维护数据处理线程
 */

#ifndef ADIWORKFLOW_H_
#define ADIWORKFLOW_H_

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <string>
#include <vector>

#include "Parameter.hpp"
#include "ImageFrame.hpp"
#include "ADIReduce.h"
#include "AMatchCatalog.h"
#include "ACalibrateFlux.h"
#include "AAssociateMotion.h"

enum {
	MODE_ZERO = 1,	/// 合并本底
	MODE_DARK,		/// 合并暗场, 依赖本底
	MODE_FLAT		/// 合并平场, 依赖本底和/或暗场
} COMBINE_MODE;

class ADIWorkFlow {
public:
	ADIWorkFlow();
	virtual ~ADIWorkFlow();

protected:
	typedef boost::shared_ptr<boost::thread> threadptr;
	typedef boost::unique_lock<boost::mutex> mutex_lock;
	typedef std::vector<std::string> strvec;

protected:
	Parameter* param_;	/// 配置参数
	int combine_;	/// 合并模式

	strvec vecCombine_;	/// 参与合并的图像文件路径

	/* 以队列维护参与不同处理流程的数据队里 */
	ImgFrmDeque dequeDIP_;		/// 图像处理队列
	ImgFrmDeque dequeAstro_;	/// 天文定位队列
	ImgFrmDeque dequePhoto_;	/// 天文测光队列
	ImgFrmDeque dequeMotio_;	/// 运动关联队列
	/* 以线程实现对不同处理流程的并行 */
	threadptr thrd_dip_;		/// 图像处理线程
	threadptr thrd_astro_;		/// 天文定位线程
	threadptr thrd_photo_;		/// 天文测光线程
	threadptr thrd_motio_;		/// 运动关联线程
	/* 条件变量触发推动处理流程 */
	boost::condition_variable cv_dip_;		/// 事件: 图像处理
	boost::condition_variable cv_astro_;	/// 事件: 天文定位
	boost::condition_variable cv_photo_;	/// 事件: 天文测光
	boost::condition_variable cv_motio_;	/// 事件: 运动关联

public:
	/*!
	 * @brief 启动数据处理流程服务
	 * @param param 配置参数
	 * @return
	 * 服务启动结果
	 */
	bool Start(Parameter* param);
	/*!
	 * @brief 停止数据处理流程服务
	 */
	void Stop();
	/*!
	 * @brief 合并前准备
	 */
	void BeginCombine(Parameter* param, int mode);
	/*!
	 * @brief 添加用于合并的图像文件
	 * @param filePath 文件路径
	 */
	void AddCombineImage(const char* filePath);
	/*!
	 * @brief 完成合并操作流程
	 */
	void EndCombine();
	/*!
	 * @brief 处理单帧图像文件
	 * @param filePath 文件路径
	 */
	void ProcessImage(const char* filePath);

protected:
	/* 线程接口 */
	/*!
	 * @brief 线程: 图像处理, 从图像数据中提取/测量信号和目标
	 */
	void thread_dip();
	/*!
	 * @brief 线程: 天文定位
	 */
	void thread_astro();
	/*!
	 * @brief 线程: 流量定标/较差测光
	 */
	void thread_photo();
	/*!
	 * @brief 线程: 关联识别运动目标/光变目标
	 */
	void thread_motio();
	/*!
	 * @brief 中断线程
	 * @param thrd 线程指针
	 */
	void interrupt_thread(threadptr& thrd);
};

#endif /* ADIWORKFLOW_H_ */
