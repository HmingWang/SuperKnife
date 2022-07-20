package com.whaim.superknife.controller;

import com.ibm.jms.JMSMessage;
import com.ibm.mq.jms.MQQueue;
import com.ibm.mq.jms.MQQueueConnectionFactory;
import com.ibm.msg.client.wmq.WMQConstants;
import com.whaim.superknife.common.ApiResult;
import lombok.extern.slf4j.Slf4j;
import com.ibm.mq.jms.MQConnectionFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.jms.annotation.JmsListener;
import org.springframework.jms.connection.CachingConnectionFactory;
import org.springframework.jms.core.JmsTemplate;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import javax.jms.*;
import javax.servlet.http.HttpServletRequest;

@Slf4j
@RestController
public class MQController {

    @Autowired
    HttpServletRequest request;

//    @Autowired
//    JmsTemplate jmsTemplate;

    @GetMapping("init")
    ApiResult init(@RequestParam String qmgr,
                   @RequestParam String channel,
                   @RequestParam String conn,
                   @RequestParam String userid,
                   @RequestParam String password,
                   @RequestParam String qsend,
                   @RequestParam String qrecv) {
        try {
            MQQueueConnectionFactory qcf = new MQQueueConnectionFactory();
            qcf.setQueueManager(qmgr);
            qcf.setChannel(channel);
            qcf.setConnectionNameList(conn);
            qcf.setTransportType(WMQConstants.WMQ_CM_CLIENT);
            QueueConnection qc = qcf.createQueueConnection(userid, password);
            qc.start();

            QueueSession qs = qc.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);

            Queue qSend = new MQQueue(qsend);
            QueueSender sender = qs.createSender(qSend);

            Queue qRecv = new MQQueue(qrecv);
            QueueReceiver receiver = qs.createReceiver(qRecv);

            request.getSession().setAttribute("qsend", sender);
            request.getSession().setAttribute("qrecv", receiver);
            request.getSession().setAttribute("qsession", qs);
            log.info("mq connect success!");
            return ApiResult.success();
        } catch (JMSException e) {
            e.printStackTrace();
            log.error(e.getMessage());
            return ApiResult.failed(e.getMessage());
        }
    }

    @GetMapping("send")
    ApiResult send(@RequestParam String msg) {
        QueueSender s = (QueueSender) request.getSession().getAttribute("qsend");
        QueueSession qs = (QueueSession) request.getSession().getAttribute("qsession");
        try {
            Message m = qs.createTextMessage(msg);
            s.send(m);
            log.info("send message::{}", msg);

            return ApiResult.success();
        } catch (JMSException e) {
            log.error(e.getMessage());
            return ApiResult.failed(e.getMessage());
        }
    }

    @GetMapping("recv")
    ApiResult recv() {
        QueueReceiver r = (QueueReceiver) request.getSession().getAttribute("qrecv");
        try {
            Message m = r.receiveNoWait();
            String msg="";

            if (m != null) {
                msg = m.getBody(String.class);
                log.info("recv message::{}", msg);
            }
            return ApiResult.success(msg);
        } catch (JMSException e) {
            log.error(e.getMessage());
            return ApiResult.failed(e.getMessage());
        }
    }

}
