package com.yugabyte.yw.forms;

import com.fasterxml.jackson.annotation.JsonFormat;
import io.swagger.annotations.ApiModel;
import io.swagger.annotations.ApiModelProperty;
import java.util.Date;

@ApiModel(
    description =
        "The response type for triggering a health check. "
            + "Contains the timestamp of when the health check was triggered.")
public class TriggerHealthCheckResult {

  @ApiModelProperty(value = "The ISO-8601 timestamp when the health check was triggered.")
  @JsonFormat(shape = JsonFormat.Shape.STRING, pattern = "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'")
  public Date timestamp;
}
