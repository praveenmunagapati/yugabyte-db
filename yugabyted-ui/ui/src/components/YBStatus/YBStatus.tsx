import React, { FC, ReactNode } from 'react';
import { FiberManualRecord } from '@material-ui/icons';
import { Box, createStyles, makeStyles, Theme } from '@material-ui/core';
import FailedIcon from '@app/assets/failed-solid.svg';
import CompletedIcon from '@app/assets/check.svg';
import SuccessIcon from '@app/assets/circle-check-solid.svg';
import LoadingIcon from '@app/assets/Default-Loading-Circles.svg';

export enum STATUS_TYPES {
  SUCCESS = 'success',
  FAILED = 'failed',
  COMPLETE = 'completed',
  ACTIVE = 'active',
  INACTIVE = 'inactive',
  PENDING = 'running',
  IN_PROGRESS = 'in_progress'
}

interface StatusProps {
  label?: ReactNode;
  type?: STATUS_TYPES;
}

const useStyles = makeStyles((theme: Theme) => {
  return createStyles({
    root: {
      textAlign: 'center',
      '& > span': {
        margin: theme.spacing(1)
      }
    },
    colorCompleted: {
      marginRight: theme.spacing(0.5),
      color: theme.palette.success.main
    },
    colorSuccess: {
      marginRight: theme.spacing(0.5),
      color: theme.palette.success.main
    },
    colorInactive: {
      fontSize: 12,
      color: theme.palette.grey[500]
    },
    colorActive: {
      fontSize: 12,
      marginRight: theme.spacing(0.5),
      color: theme.palette.success.main
    },
    colorFailed: {
      width: 24,
      marginRight: theme.spacing(0.5),
      color: theme.palette.error.main
    },
    colorPending: {
      marginRight: theme.spacing(0.5),
      width: 12,
      color: theme.palette.warning[700]
    },
    loadingIcon: {
      width: 21,
      height: 21,
      margin: 0,
      marginRight: theme.spacing(1)
    }
  });
});

export const YBStatus: FC<StatusProps> = ({ label, type = STATUS_TYPES.COMPLETE }: StatusProps) => {
  const classes = useStyles();

  const getIcon = () => {
    switch (type) {
      case STATUS_TYPES.FAILED: {
        return <FailedIcon className={classes.colorFailed} />;
      }
      case STATUS_TYPES.ACTIVE: {
        return <FiberManualRecord className={classes.colorActive} />;
      }
      case STATUS_TYPES.INACTIVE: {
        return <FiberManualRecord className={classes.colorInactive} />;
      }
      case STATUS_TYPES.SUCCESS: {
        return <CompletedIcon className={classes.colorSuccess} />;
      }
      case STATUS_TYPES.PENDING: {
        return <FiberManualRecord className={classes.colorPending} />;
      }
      case STATUS_TYPES.IN_PROGRESS: {
        return <LoadingIcon className={classes.loadingIcon} />;
      }
      default: {
        return <SuccessIcon className={classes.colorCompleted} />;
      }
    }
  };

  return (
    <Box display="flex" alignItems="center" justifyContent="center">
      {getIcon()}
      {label && <Box minWidth={35}>{label}</Box>}
    </Box>
  );
};
