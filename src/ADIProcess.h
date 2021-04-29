/*!
 * @class ADIProcess 天文数字图像处理基类
 * @version 0.1
 * @date 2021-04
 */

#ifndef SRC_ADIPROCESS_H_
#define SRC_ADIPROCESS_H_

#include <boost/thread/thread.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include "ImageFrame.hpp"
#include "Parameter.hpp"

class ADIProcess {
public:
	ADIProcess(Parameter* param);
	virtual ~ADIProcess();

public:
	typedef boost::signals2::signal<void (bool)> CBResult;	///< 处理结果回调函数
	typedef CBResult::slot_type CBResultSlot;			///< 处理结果回调函数插槽
	typedef boost::shared_ptr<boost::thread> threadptr;	///< 线程指针

protected:
	Parameter* param_;		/// 配置参数
	bool working_;			/// 工作标志
	threadptr thrd_proc_;	/// 线程: 在线程中运行处理过程
	string nameFunc_;		/// 功能名称
	CBResult cbRslt_;		/// 回调函数
	ImgFrmPtr frame_;		/// 图像帧

public:
	/*!
	 * @brief 注册处理结果回调函数
	 * @param slot 函数插槽
	 */
	void RegisterResult(const CBResultSlot &slot);
	/*!
	 * @brief 检测是否仍在执行处理流程处理流程
	 * @return
	 * 执行标志
	 */
	bool IsWorking();
	/*!
	 * @brief 获取图像帧
	 * @return
	 * 当前图像帧
	 */
	ImgFrmPtr GetFrame();
	/*!
	 * @brief 处理单帧图像
	 * @param frame    图像帧
	 * @param imgData  图像数据存储地址
	 */
	bool DoIt(ImgFrmPtr frame);

protected:
	/*!
	 * @brief 以多线程方式运行处理过程
	 */
	void thread_process();
	/*!
	 * @brief 在多进程模式下执行真正的处理流程
	 * @param imgData  图像数据存储地址
	 * @return
	 * 处理成功标志
	 */
	virtual bool do_real_process() = 0;
};

#endif /* SRC_ADIPROCESS_H_ */
