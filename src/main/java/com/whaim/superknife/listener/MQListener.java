package com.whaim.superknife.listener;

import lombok.extern.slf4j.Slf4j;
import org.springframework.jms.annotation.JmsListener;
import org.springframework.stereotype.Component;

import javax.jms.JMSException;

@Slf4j
@Component
public class MQListener {

    //@JmsListener(destination = "DEV.QUEUE.1")
    void recv(javax.jms.Message message) {
        String msg = null;
        try {
            msg = message.getBody(String.class);
        } catch (JMSException e) {
            throw new RuntimeException(e);
        }
        log.info("recv message::{}",msg);

    }

}
